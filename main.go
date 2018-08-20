package main

/*
#cgo CFLAGS: -I. -I./lib
#cgo LDFLAGS: -L. -L./lib -Wl,-rpath,$ORIGIN/lib -lgalvo
#include <galvo.h>
*/
import "C"
import "fmt"

func main() {
	C.initGalvo()
	fmt.Println("hello world")
	C.closeGalvo()
}
