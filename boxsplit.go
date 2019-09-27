package main

import (
  "bufio"
  "fmt"
  "image"
  "image/png"
  "log"
  "os"
  "strings"
  "strconv"
)

type SubImager interface {
  SubImage(r image.Rectangle) image.Image
  Bounds() image.Rectangle
}

func main() {
  if len(os.Args) <= 2 {
    return
  }
  pngFileName := os.Args[1]
  pngFile, err := os.Open(pngFileName)
  if err != nil {
    log.Fatal(err)
  }
  img, err := png.Decode(pngFile)
  if err != nil {
    log.Fatal(err)
  }
  imgX, ok := (img).(SubImager)
  if !ok {
    log.Fatal("no subimager")
  }
  bounds := imgX.Bounds()
  log.Println(bounds)
  fi, err := os.Open(os.Args[2])
  if err != nil {
    log.Fatal(err)
  }
  defer fi.Close()
  scanner := bufio.NewScanner(fi)
  i := 0
  for scanner.Scan() {
    line := scanner.Text()
    if line[0] != '\t' { continue }
    parts := strings.Split(line[2:len(line)], " ")
    log.Println(parts)
    x0, _ := strconv.Atoi(parts[0])
    y0, _ := strconv.Atoi(parts[1])
    x1, _ := strconv.Atoi(parts[2])
    y1, _ := strconv.Atoi(parts[3])
    y0 = bounds.Max.Y - y0
    y1 = bounds.Max.Y - y1
    r := image.Rectangle{image.Point{x0, y1}, image.Point{x1, y0}}
    log.Println(r)
    subImg := imgX.SubImage(r)
    fo, err := os.Create(fmt.Sprintf("%03d.png", i))
    if err != nil {
      log.Fatal(err)
    }
    defer fo.Close()
    if err := png.Encode(fo, subImg); err != nil {
      log.Fatal(err)
    }
    i++
  }
  //fo.Flash()
}
