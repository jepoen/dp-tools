package main

import (
  "bufio"
  "encoding/json"
  "errors"
  "fmt"
  "io/ioutil"
  "log"
  "os"
  "path/filepath"
  "regexp"
  "sort"
  "strconv"
  "strings"
  "unicode"
  "unicode/utf8"
)

type Params struct {
  Longs_ratio float64        // h/w > long_ratio: ſ -> f at word end
  S_speckle_hratio float64   // h < s_speckle_ratio * median(hLetter) -> remove
  Nospace_ratio float64      // space < nospace_ratio * median(wSpace) ->remove
}

var params Params = Params{ 2.0, 0.2, 0.3 }

type Quartiles struct {
  min int
  q1  int
  med int
  q3  int
  max int
}

type Box struct {
  glyph string
  x0 int
  y0 int
  x1 int
  y1 int
}

type BoxLine struct {
  boxes []Box
  wLetter Quartiles
  hLetter Quartiles
  wSpace Quartiles
  wOther Quartiles
  hOther Quartiles
}

var logger *log.Logger

type stringlist_t []string
type wordcount_t struct {
  word string
  count int
}

type summary_t struct {
  replacements map[string]wordcount_t
}

var summary summary_t

const (
  FONT_NONE = iota
  FONT_ANTIQUA
  FONT_FRAKTUR
)

const (
  OPT_FONT = iota
  OPT_JOIN
  OPT_RAW
  OPT_RENUMBER
  OPT_REPLACE_SF
)

func init() {
  logFile, err := os.Create("handleTess.log")
  if err != nil {
    logger = log.New(os.Stderr, "", 0)
    logger.Printf("Cannot open logfile: %s\n", err)

  } else {
    logger = log.New(logFile, "", 0)
  }
  logger.Println("Logger started")
  summary = NewSummary()
}

func (q Quartiles) String() string {
  return fmt.Sprintf("min: %d q1: %d med: %d q3: %d max: %d",
    q.min, q.q1, q.med, q.q3, q.max)
}

func writeParams() {
  b, err := json.MarshalIndent(params, "", "  ")
  if err != nil {
    logger.Fatal(err)
  }
  fo, err := os.Create("boxparams.dat")
  if err != nil {
    logger.Fatal(err)
  }
  defer fo.Close()
  fo.Write(b)
}

func readParams() {
  if _, err := os.Stat("boxparams.dat"); os.IsNotExist(err) {
    writeParams()
  }
  fi, err := os.Open("boxparams.dat")
  defer fi.Close()
  b, ferr := ioutil.ReadAll(fi)
  if ferr != nil {
    logger.Fatal(err)
  }
  err = json.Unmarshal(b, &params)
  if err != nil {
    logger.Fatal(err)
  }
}

func (box *Box) wGlyph() int {
  if box.x0 < 0 { return -1 }
  return box.x1 - box.x0
}

func (box *Box) hGlyph() int {
  if box.y0 < 0 { return -1 }
  return box.y1 - box.y0
}

func intQuartiles (vals []int) Quartiles {
  if len(vals) == 0 {
    return Quartiles{-1, -1, -1, -1, -1}
  }
  sort.Ints(vals)
  l := len(vals)-1
  return Quartiles{vals[0], vals[l/4], vals[l/2], vals[3*l/4], vals[l]}
}

func newBoxLine(boxes []Box) BoxLine {
  wLetter := []int{}
  hLetter := []int{}
  wOther := []int{}
  hOther := []int{}
  wSpace := []int{}
  for idx, box := range boxes {
    if w := box.wGlyph(); w >= 0 {
      h := box.hGlyph()
      r,_ := utf8.DecodeRuneInString(box.glyph)
      if unicode.IsLetter(r) || unicode.IsNumber(r) {
        wLetter = append(wLetter, w)
        hLetter = append(hLetter, h)
      } else {
        wOther = append(wOther, w)
        hOther = append(hOther, h)
      }
    } else if box.glyph == " " && idx > 0 && idx < len(boxes)-1 {
      boxes[idx].x0 = boxes[idx-1].x1
      boxes[idx].x1 = boxes[idx+1].x0
      space := boxes[idx].x1 - boxes[idx].x0
      logger.Printf("Space x0: %d x1: %d space: %d\n", boxes[idx].x0,
        boxes[idx].x1, space)
      wSpace = append(wSpace, space)
    }
  }
  return BoxLine{boxes, intQuartiles(wLetter), intQuartiles(hLetter),
    intQuartiles(wSpace), intQuartiles(wOther), intQuartiles(hOther)}
}

func isWordEnd(boxes []Box, idx int) bool {
  if idx == len(boxes)-1 { return true }
  box := boxes[idx+1]
  r, _ := utf8.DecodeRuneInString(box.glyph)
  return !unicode.IsLetter(r)
}

func (boxLine *BoxLine) handle_space() {
  res := []Box{}
  for idx, box := range(boxLine.boxes) {
    if box.glyph != " " {
      res = append(res, box)
      continue
    }
    //logger.Printf("handle_space %d %v\n", idx, boxLine.boxes[idx])
    bSpace := boxLine.boxes[idx]
    wSpace := bSpace.x1 - bSpace.x0
    if wSpace < 0 { wSpace = boxLine.wSpace.max }
    if float64(wSpace) > params.Nospace_ratio*float64(boxLine.wSpace.med) {
      res = append(res, box)
    } else {
      logger.Printf("Space after s %d median %d removed\n", wSpace,
        boxLine.wSpace.med)
    }
  }
  boxLine.boxes = res
}

func (boxLine *BoxLine) handleSlimLetter() {
  slimLetters := map[string]bool{"s": true, "l": true, "i": true}
  res := []Box{}
  for _, box := range(boxLine.boxes) {
    if _, ok := slimLetters[box.glyph]; ok {
      h := box.hGlyph()
      // Speckle
      if float64(h) < params.S_speckle_hratio * float64(boxLine.hLetter.med) {
        continue
      }
    }
    res = append(res, box)
  }
  oldText := boxes2text(boxLine.boxes)
  newText := boxes2text(res)
  if oldText != newText {
    logger.Printf("handle slim letters:\n- %s\n+ %s",
      oldText, newText)
  }
  boxLine.boxes = res
}

func (boxLine *BoxLine) handle_s() {
  res := []Box{}
  for idx, box := range(boxLine.boxes) {
    if box.glyph != "s" {
      res = append(res, box)
      continue
    }
    w := box.wGlyph()
    h := box.hGlyph()
    ratio := float64(h)/float64(w)
    logger.Printf("handle s: pos: %d w %d h %d r %.1f thr %.1f\n",
      idx, w, h, ratio, params.Longs_ratio)
    if ratio < params.Longs_ratio {
      res = append(res, box)
      continue
    }
    /* in handleSlimLetter()
    // long ſ or speckle
    if float64(h) < params.S_speckle_hratio * float64(boxLine.hLetter.med) {
      // speckle
      logger.Println("Speckle removed")
      continue
    }
    */
    if isWordEnd(boxLine.boxes, idx) {
      logger.Println("ſ -> f")
      box.glyph = "f"
    }
    res = append(res, box)
  }
  boxLine.boxes = res
}

func readBoxes(txtFile, boxFile string) ([]string, []Box) {
  ftxt, err2 := os.Open(txtFile)
  txt := []string{}
  if err2 != nil {
    log.Println(err2)
    return txt, nil
  }
  defer ftxt.Close()
  txtScanner := bufio.NewScanner(ftxt)
  for txtScanner.Scan() {
    txt = append(txt, txtScanner.Text())
  }
  if _, err := os.Stat(boxFile); os.IsNotExist(err) {
    log.Println("No box file", boxFile)
    return txt, nil
  }
  fbox, err := os.Open(boxFile)
  if err != nil {
    fmt.Println(err)
    return txt, nil
  }
  defer fbox.Close()
  boxScanner := bufio.NewScanner(fbox)
  boxes := []Box{}
  for boxScanner.Scan() {
    parts := strings.Split(boxScanner.Text(), " ")
    glyph := parts[0]
    pos := make([]int, 4)
    for i, p := range(parts[1:5]) {
      pos[i], err = strconv.Atoi(p)
    }
    boxes = append(boxes, Box{glyph, pos[0], pos[1], pos[2], pos[3]})
  }
  return txt, boxes
}

func matchBoxes(boxes []Box, lines []string) []Box {
  res := []Box{}
  part := strings.Join(lines, "\n")
  for _, box := range(boxes) {
    i := strings.Index(part, box.glyph)
    if i < 0 {
      logger.Printf("Glyph %s not in string\n", box.glyph)
      continue
    } else if i > 0 {
      for _, r := range(part[:i]) {
        res = append(res, Box{string(r), -1, -1, -1, -1})
      }
      if part[:i] != " " {
        logger.Printf("No Box for %q\n", part[:i])
      }
    }
    res = append(res, box)
    part = part[i+len(box.glyph):]
  }
  if part != "" {
    for _, r := range(part) {
      res = append(res, Box{string(r), -1, -1, -1, -1})
    }
    logger.Printf("No Box for %q\n", part)
  }
  return res
}

func boxes2text(boxes []Box) string {
  res := []string{}
  for _, box := range(boxes) {
    res = append(res, box.glyph)
  }
  return strings.Join(res, "")
}

func boxes2lines(boxes []Box) []BoxLine {
  res := []BoxLine{}
  startIdx := 0
  for i, box := range(boxes) {
    if box.glyph == "\n" {
      res = append(res, newBoxLine(boxes[startIdx:i]))
      startIdx = i+1
    }
  }
  if startIdx < len(boxes)-1 {
    res = append(res, newBoxLine(boxes[startIdx:]))
  }
  for _, boxes := range(res) {
    logger.Println(boxes2text(boxes.boxes))
    logger.Println("wLetter: ", boxes.wLetter,
      "  hLetter: ", boxes.hLetter)
    logger.Println("wSpace: ", boxes.wSpace)
    logger.Println("wOther: ", boxes.wOther,
      "  hOther: ", boxes.hOther)
  }
  return res
}

func medStartPos(boxLines []BoxLine) (Quartiles, error) {
  starts := []int{}
  for _, boxLine := range(boxLines) {
    x0 := -1
    for _, box := range(boxLine.boxes) {
      if box.x0 >= 0 {
        x0 = box.x0
        break
      }
    }
    if x0 >= 0 {
      starts = append(starts, x0)
    }
    //logger.Printf("%d %q\n", x0, boxes2text(boxLine.boxes))
  }
  if len(starts) == 0 {
    return Quartiles{-1, -1, -1, -1, -1}, errors.New("empty")
  }
  return intQuartiles(starts), nil
}

func processBoxes(lines []string, boxes []Box) []string {
  boxes = matchBoxes(boxes, lines)
  //boxes = evalBoxes(boxes)
  text := strings.Join(lines, "\n")
  boxText := boxes2text(boxes)
  logger.Println("isEqual: ", text==boxText)
  boxLines := boxes2lines(boxes)
  res := []string{}
  for _, boxLine := range(boxLines) {
    oldText := boxes2text(boxLine.boxes)
    logger.Println("-", oldText)
    boxLine.handle_space()
    boxLine.handleSlimLetter()
    boxLine.handle_s()
    newText := boxes2text(boxLine.boxes)
    if oldText != newText {
      logger.Println("+", newText)
    }
    res = append(res, newText)
  }
  q, err := medStartPos(boxLines)
  if err == nil {
    logger.Println("linestart: ", q)
  }
  return res
}

func NewSummary() summary_t {
  var summary summary_t
  summary.replacements = make(map[string]wordcount_t)
  return summary
}

func LogAddReplacement(word string, newWord string) {
  e, ok := summary.replacements[word]
  if ok {
    if e.word != newWord {
      fmt.Println("Irregular replacement", word, e.word, newWord)
    }
    e.count++
  } else {
    e.word = newWord
    e.count = 1
  }
  summary.replacements[word] = e
}

func containsRune(runes [] rune, r rune) bool {
  for _, rr := range runes {
    if rr == r { return true; }
  }
  return false;
}

func joinLines(text stringlist_t) stringlist_t {
  caps := make([]rune, 0, 29)
  for r := 'A'; r <= 'Z'; r++ { caps = append(caps, r) }
  caps = append(caps, 'Ä', 'Ö', 'Ü')
  // join char is ASCII = 1 byte
  for i := 0; i < len(text)-1; i++ {
    line := text[i]
    pLast := len(line)-1
    if pLast < 1 { continue }
    if line[pLast] != '-' || line[pLast-1] == '-' { continue }
    nline := text[i+1]
    if len(strings.Trim(nline, " \t")) == 0 { continue }
    pos := -1
    var first rune
    for i, r := range nline {
      if i == 0 { first = r }
      if r == ' ' {
        pos = i
	break
      }
    }
    //fmt.Println("before:", line, "||", nline)
    // don't remove dash if following line starts with cap
    noRemove := 0
    if containsRune(caps, first) { noRemove = 1 }
    if pos > 0 {
      text[i] = line[0:pLast+noRemove]+nline[0:pos]
      text[i+1] = nline[pos+1:]
    } else {
      text[i] = line[0:pLast+noRemove]+nline
      text[i+1] = ""
    }
    //fmt.Println("after:", text[i], "||", text[i+1])
  }
  return text
}

/**
 * Remove repeated blank lines
 */
func removeBlankLines(text stringlist_t) stringlist_t {
  ntext := make([]string, 0, len(text))
  prevBlank := false
  for _, line := range text {
    line = strings.Trim(line, " \t")
    if line == "" {
      if prevBlank { continue }
      prevBlank = true
    } else {
      prevBlank = false
    }
    ntext = append(ntext, line)
  }
  return ntext
}

/**
 * splits line into wordsep, word, and remaining line
 */
func nextWord(line string) (string, string, string) {
  sep := ""
  word := ""
  rest := ""
  state := 0
  wordStart := -1
  wordEnd := -1
  for i, r := range line {
    if unicode.IsLetter(r) && state == 0 {
      wordStart = i
      sep = line[0:wordStart]
      state = 1
    } else if !unicode.IsLetter(r) && state == 1 {
      wordEnd = i
      word = line[wordStart:wordEnd]
      rest = line[wordEnd:]
      state = 2
    }
  }
  if state == 0 {
    return line, "", ""
  } else if state == 1 {
    word = line[wordStart:]
    return sep, word, ""
  } else {
    return sep, word, rest
  }
}

func handleSepFrak(sep string) string {
  ns := sep
  patterns := [][]string{
    {`,,`, `»`},
    {` +([!?:,;:i«)])`, `$1`},
    {`([»(]) +`, `$1`},
    {`-{3,}`, `--`},
    {` *\. \. \.`, ` ...`},
    //{` {2,}`, ` `},  // double blanks indicate missing periods etc.
  }
  for _, entry := range(patterns) {
    re := regexp.MustCompile(entry[0])
    ns = re.ReplaceAllString(ns, entry[1])
  }
  if sep != ns {
    LogAddReplacement(sep, ns)
  }
  return ns
}

func handleSepAntiqua(sep string) string {
  ns := sep
  patterns := [][]string{
    {`,,`, `»`},
    {`-{3,}`, `--`},
  }
  for _, entry := range(patterns) {
    re := regexp.MustCompile(entry[0])
    ns = re.ReplaceAllString(ns, entry[1])
  }
  if sep != ns {
    LogAddReplacement(sep, ns)
  }
  return ns
}

func handleSep(sep string, font int) string {
  if font == FONT_FRAKTUR {
    return handleSepFrak(sep)
  } else {
    return handleSepAntiqua(sep)
  }
}

func handleWordFrak(word string) string {
  nw := word
  patterns := [][]string{
    {`^muffen`, `müssen`},
    {`uug$`, `ung`},
    {`nng$`, `ung`},
    {`geung$`, `genug`}, // repair above replacement
    {"tft", "tst"},
    {"uiß", "niß"},
    {"^fp", "sp"},
    {"^ft", "st"},
    {"Ea", "Ca"},
    {`Ee`, `Ce`},
    {`Ehrist`, `Christ`},
    {`^fch`, `sch`},
    {`fch$`, `sch`},
    {`Qn`, `Qu`},
    {`qn`, `qu`},
    {`^fo$`, `so`},
    {`^ift$`, `ist`},
    {`^uud$`, `und`},
    {`^nnd$`, `und`},
    {`^fich$`, `sich`},
    {`^foll`, `soll`},
    {`^zn`, `zu`},
    {`[sf]chast$`, `schaft`},
    {"J([bcdfghjklmnpqrstvwxzß])", "I${1}"},
    {"I([aeiouyäöü])", "J${1}"},
  }
  for _, entry := range(patterns) {
    re := regexp.MustCompile(entry[0])
    nw = re.ReplaceAllString(nw, entry[1])
  }
  if word != nw {
    LogAddReplacement(word, nw)
  }
  return nw
}

func handleWordAntiqua(word string) string {
  nw := word
  patterns := [][]string{
  }
  for _, entry := range(patterns) {
    re := regexp.MustCompile(entry[0])
    nw = re.ReplaceAllString(nw, entry[1])
  }
  if word != nw {
    LogAddReplacement(word, nw)
  }
  return nw
  return word
}

func handleWord(word string, font int) string {
  if font == FONT_FRAKTUR {
    return handleWordFrak(word)
  } else {
    return handleWordAntiqua(word)
  }
}

func handleWords(line string, font int) string {
  newLine := ""
  for len(line) > 0 {
    sep, word, rest := nextWord(line)
    line = rest
    newLine += handleSep(sep, font)+handleWord(word, font)
  }
  return newLine
}

func handleLine(line string, font int) string {
  nl := line
  patterns := [][]string{
    {`^ \.\.\.`, `...`},
  }
  frakPatterns := [][]string{
    {` f\.`, ` s.`},
  }
  for _, entry := range(patterns) {
    re := regexp.MustCompile(entry[0])
    nl = re.ReplaceAllString(nl, entry[1])
  }
  for _, entry := range(frakPatterns) {
    re := regexp.MustCompile(entry[0])
    nl = re.ReplaceAllString(nl, entry[1])
  }
  if nl != line {
    LogAddReplacement(line, nl)
  }
  return nl
}

func replaceRune(r rune) string {
  repl := map[rune]string {
    '“': "«",
    '„': "»",
    '—': "--",
    '–': "--",
    'ﬂ': "fl",
    'ﬁ': "fi",
    '\u201a': ",",
    '\u2019': "'",
    '\u2018': "'",
  }
  return repl[r]
}

func prepare(lines []string, opts map[int]int) []string {
  text := []string{}
  for _, line := range(lines) {
    runes := []rune{}
    for _, r := range line {
      if r >= 32 && r <= 256 {
        runes = append(runes, r)
      } else {
        repl := replaceRune(r)
        if repl == "" {
          fmt.Printf("%x %c\n", r, r)
          runes = append(runes, '@')
        } else {
          for _, r := range(repl) {
            runes = append(runes, r)
          }
        }
      }
    }
    text = append(text, string(runes))
    //fmt.Println(text)
  }
  if opts[OPT_JOIN] != 0 {
    text = joinLines(text)
  }
  text = removeBlankLines(text)
  newText := make(stringlist_t, 0, len(text))
  for _, line := range text {
    if opts[OPT_RAW] == 0 {
      line = handleWords(line, opts[OPT_FONT])
      line = handleLine(line, opts[OPT_FONT])
    }
    newText = append(newText, line)
  }
  return newText
}

func handleFile(base, outBase string, opts map[int]int) {
  inFile := filepath.Join("in", base+".txt")
  boxFile := filepath.Join("in", base+".box")
  outFile := filepath.Join("out", outBase+".txt")
  if opts[OPT_RENUMBER] == 0 {
    outFile = filepath.Join("out", base+".txt")
  }
  lines, boxes := readBoxes(inFile, boxFile)
  if boxes != nil && opts[OPT_REPLACE_SF] == 1 {
    lines = processBoxes(lines, boxes)
  }
  fmt.Printf("%s -> %s\n", inFile, outFile)
  logger.Printf("%s -> %s\n", inFile, outFile)
  lines = prepare(lines, opts)
  // Write file
  fo, err := os.Create(outFile)
  if err != nil {
    fmt.Println(err)
    return
  }
  defer fo.Close()
  writer := bufio.NewWriter(fo)
  for _, line := range lines {
    buffer := make([]byte, 0, len(line))
    for _, r := range line {
      buffer = append(buffer, byte(r))
    }
    writer.Write(buffer)
    writer.WriteRune('\n')
    writer.Flush()
  }
}

func handleFiles(opts map[int]int) {
  filelist, err := ioutil.ReadDir("in")
  if err != nil {
    logger.Println(err)
    return
  }
  err = os.MkdirAll("out", os.ModePerm)
  if err != nil {
    logger.Println(err)
    return
  }
  i := 0
  for _, fi := range filelist {
    fname := fi.Name()
    ext := filepath.Ext(fname)
    base := fname[:len(fname)-len(ext)]
    logger.Println("File", i, fname, base, ext)
    if ext == ".txt" {
      i++
      outBase := fmt.Sprintf("%03d", i)
      handleFile(base, outBase, opts)
    }
  }
}

func printSummary() {
  fmt.Println("Replacements")
  sum := 0
  keys := make([]string, 0, len(summary.replacements))
  for k := range summary.replacements {
    keys = append(keys, k)
  }
  sort.Strings(keys)
  for _, k := range keys {
    v := summary.replacements[k]
    fmt.Printf("%s -> %s (%d)\n", k, v.word, v.count)
    logger.Printf("%s -> %s (%d)\n", k, v.word, v.count)
    sum += v.count
  }
  fmt.Printf("Sum of replacements: %d\n", sum)
  logger.Printf("Sum of replacements: %d\n", sum)
}

func getOpt(args []string) map[int]int {
  opts := map[int]int{OPT_FONT: FONT_NONE, OPT_JOIN: 1, OPT_RENUMBER: 1,
    OPT_REPLACE_SF: 1, OPT_RAW: 0}
  for _, arg := range(args) {
     switch arg {
       case "-f": opts[OPT_FONT] = FONT_FRAKTUR
       case "-a": opts[OPT_FONT] = FONT_ANTIQUA
       case "-j": opts[OPT_JOIN] = 0
       case "-n": opts[OPT_RENUMBER] = 0
       case "-s": opts[OPT_REPLACE_SF] = 0
       case "-p": opts[OPT_RAW] = 1
     }
  }
  if opts[OPT_RAW] == 1 { opts[OPT_JOIN] = 0 }
  if opts[OPT_FONT] == FONT_ANTIQUA { opts[OPT_REPLACE_SF] = 0 }
  return opts
}

func usage(cmd string) {
  fmt.Printf("Usage: %s flag\n", cmd)
  fmt.Println("Flags: -f -- Fraktur")
  fmt.Println("       -a -- Antiqua")
  fmt.Println("       -j -- Don't join words")
  fmt.Println("       -n -- Don't renumber files")
  fmt.Println("       -s -- Don't handle s/f (Fraktur only)")
  fmt.Println("       -p -- No prepare text (convert only Unicode chars)")
}

func main() {
  readParams()
  args := os.Args[1:]
  opts := getOpt(args)
  if opts[OPT_FONT] == FONT_NONE && opts[OPT_RAW] == 0 {
    usage(os.Args[0])
    os.Exit(1)
  }
  handleFiles(opts)
  printSummary()
}
