package main

import (
  "bufio"
  "fmt"
  "io/ioutil"
  "os"
  "path/filepath"
  "regexp"
  "sort"
  "strings"
  "unicode"
)

type stringlist_t []string
type wordcount_t struct {
  word string
  count int
}

type log_t struct {
  replacements map[string]wordcount_t
}

var log log_t

const (
  FONT_NONE = iota
  FONT_ANTIQUA
  FONT_FRAKTUR
)

const (
  OPT_FONT = iota
  OPT_JOIN
  OPT_RAW
)

func NewLog() log_t {
  var log log_t
  log.replacements = make(map[string]wordcount_t)
  return log
}

func LogAddReplacement(word string, newWord string) {
  e, ok := log.replacements[word]
  if ok {
    if e.word != newWord {
      fmt.Println("Irregular replacement", word, e.word, newWord)
    }
    e.count++
  } else {
    e.word = newWord
    e.count = 1
  }
  log.replacements[word] = e
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

func prepare(infile string, outfile string, opts map[int]int) {
  fi, err := os.Open(infile)
  if err != nil {
    fmt.Println(err)
    return
  }
  scanner := bufio.NewScanner(fi)
  text := []string{}
  for scanner.Scan() {
    line := scanner.Text()
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
      newText = append(newText, handleWords(line, opts[OPT_FONT]))
    } else {
      newText = append(newText, line)
    }
  }
  // Write file
  fo, err := os.Create(outfile)
  if err != nil {
    fmt.Println(err)
    return
  }
  defer fo.Close()
  writer := bufio.NewWriter(fo)
  for _, line := range newText {
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
    fmt.Println(err)
    return
  }
  err = os.MkdirAll("out", os.ModePerm)
  if err != nil {
    fmt.Println(err)
    return
  }
  i := 0
  for _, fi := range filelist {
    infile := filepath.Join("in", fi.Name())
    ext := filepath.Ext(infile)
    if ext != ".txt" { continue }
    i++
    outfile := filepath.Join("out", fmt.Sprintf("%03d.txt", i))
    fmt.Println("Handle", infile, outfile)
    prepare(infile, outfile, opts)
  }
}

func printLog() {
  fmt.Println("Replacements")
  sum := 0
  keys := make([]string, 0, len(log.replacements))
  for k := range log.replacements {
    keys = append(keys, k)
  }
  sort.Strings(keys)
  for _, k := range keys {
    v := log.replacements[k]
    fmt.Printf("%s -> %s (%d)\n", k, v.word, v.count)
    sum += v.count
  }
  fmt.Printf("Sum of replacements: %d\n", sum)
}

func getOpt(args []string) map[int]int {
  opts := map[int]int{OPT_FONT: FONT_NONE, OPT_JOIN: 1, OPT_RAW: 0}
  for _, arg := range(args) {
     switch arg {
       case "-f": opts[OPT_FONT] = FONT_FRAKTUR
       case "-a": opts[OPT_FONT] = FONT_ANTIQUA
       case "-j": opts[OPT_JOIN] = 0
       case "+r": opts[OPT_RAW] = 1
     }
  }
  if opts[OPT_RAW] == 1 { opts[OPT_JOIN] = 0 }
  return opts
}

func usage(cmd string) {
  fmt.Printf("Usage: %s flag\n", cmd)
  fmt.Println("Flags: -f -- Fraktur")
  fmt.Println("       -a -- Antiqua")
  fmt.Println("       -j -- Don't join words")
  fmt.Println("       +r -- Raw text (convert only Unicode chars)")
}

func main() {
  args := os.Args[1:]
  opts := getOpt(args)
  if opts[OPT_FONT] == FONT_NONE && opts[OPT_RAW] == 0 {
    usage(os.Args[0])
    os.Exit(1)
  }
  log = NewLog()
  handleFiles(opts)
  printLog()
}
