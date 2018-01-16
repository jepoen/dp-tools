package main

import (
  "fmt"
  "io"
  "io/ioutil"
  "os"
  "path"
)

func usage(cmd string) {
  fmt.Println("Usage:", cmd, " extension targetDir")
}

// http://stackoverflow.com/questions/21060945/simple-way-to-copy-a-file-in-golang
// not used yet, we simply create symbolic links
func copyFileContents(src, dst string) (err error) {
  in, err := os.Open(src)
  if err != nil {
    return
  }
  defer in.Close()
  out, err := os.Create(dst)
  if err != nil {
    return
  }
  defer func() {
    cerr := out.Close()
    if err == nil {
      err = cerr
    }
  }()
  if _, err = io.Copy(out, in); err != nil {
    return
  }
  err = out.Sync()
  return
}

func createLink(src, dst string) (err error) {
  if _, err = os.Stat(dst); err == nil {
    os.Remove(dst)
  }
  err = os.Symlink(src, dst)
  return
}

func main() {
  args := os.Args
  if len(args) != 3 {
    usage(args[0])
    os.Exit(1)
  }
  ext := "."+args[1]
  target := args[2]
  fmt.Println("Extension ", ext, "Target dir: ", target)

  files, err := ioutil.ReadDir(".")
  if err != nil {
    os.Exit(1)
  }
  nr := 1
  for _, file := range files {
    if path.Ext(file.Name()) != ext { continue }
    tgtFile := fmt.Sprintf("%s/%03d%s", target, nr, ext)
    fmt.Println(file.Name(), "→", tgtFile)
    //err = copyFileContents(file.Name(), tgtFile)
    err = createLink(file.Name(), tgtFile)
    if err != nil {
      fmt.Println(err)
    }
    nr++
  }
}
