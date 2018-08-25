package main

/*
#cgo CFLAGS: -I. -I./lib
#cgo LDFLAGS: -L. -L./lib -Wl,-rpath,$ORIGIN/lib -lgalvo
#include <galvo.h>
*/
import "C"
import (
	. "./graph"
	"fmt"
	"github.com/davecgh/go-spew/spew"
)

/*
(*graph.Graph)(0x103243a0)({
 RawEdges: ([]graph.Edge) (len=8 cap=8) {
  (graph.Edge) {
   From: (uint64) 0,
   To: (uint64) 1,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 1,
   To: (uint64) 2,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 2,
   To: (uint64) 3,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 3,
   To: (uint64) 0,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 4,
   To: (uint64) 5,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 5,
   To: (uint64) 6,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 7,
   To: (uint64) 8,
   Weight: (float64) 0
  },
  (graph.Edge) {
   From: (uint64) 8,
   To: (uint64) 9,
   Weight: (float64) 0
  }
 },
 Vertices: (map[uint64]bool) (len=10) {
  (uint64) 1: (bool) true,
  (uint64) 3: (bool) true,
  (uint64) 5: (bool) true,
  (uint64) 7: (bool) true,
  (uint64) 9: (bool) true,
  (uint64) 0: (bool) true,
  (uint64) 2: (bool) true,
  (uint64) 4: (bool) true,
  (uint64) 6: (bool) true,
  (uint64) 8: (bool) true
 },
 VertexEdges: (map[uint64]map[uint64]float64) (len=10) {
  (uint64) 0: (map[uint64]float64) (len=2) {
   (uint64) 1: (float64) 0,
   (uint64) 3: (float64) 0
  },
  (uint64) 2: (map[uint64]float64) (len=2) {
   (uint64) 1: (float64) 0,
   (uint64) 3: (float64) 0
  },
  (uint64) 4: (map[uint64]float64) (len=1) {
   (uint64) 5: (float64) 0
  },
  (uint64) 6: (map[uint64]float64) (len=1) {
   (uint64) 5: (float64) 0
  },
  (uint64) 8: (map[uint64]float64) (len=2) {
   (uint64) 7: (float64) 0,
   (uint64) 9: (float64) 0
  },
  (uint64) 1: (map[uint64]float64) (len=2) {
   (uint64) 0: (float64) 0,
   (uint64) 2: (float64) 0
  },
  (uint64) 3: (map[uint64]float64) (len=2) {
   (uint64) 2: (float64) 0,
   (uint64) 0: (float64) 0
  },
  (uint64) 5: (map[uint64]float64) (len=2) {
   (uint64) 4: (float64) 0,
   (uint64) 6: (float64) 0
  },
  (uint64) 7: (map[uint64]float64) (len=1) {
   (uint64) 8: (float64) 0
  },
  (uint64) 9: (map[uint64]float64) (len=1) {
   (uint64) 8: (float64) 0
  }
 },
 Undirected: (bool) true,
 NegEdges: (bool) false
})
*/
/*
([]map[uint64]bool) (len=3 cap=4) {
 (map[uint64]bool) (len=4) {
  (uint64) 1: (bool) true,
  (uint64) 0: (bool) true,
  (uint64) 2: (bool) true,
  (uint64) 3: (bool) true
 },
 (map[uint64]bool) (len=3) {
  (uint64) 5: (bool) true,
  (uint64) 4: (bool) true,
  (uint64) 6: (bool) true
 },
 (map[uint64]bool) (len=3) {
  (uint64) 7: (bool) true,
  (uint64) 8: (bool) true,
  (uint64) 9: (bool) true
 }
}
*/

func main() {
	gr := GetGraph(
		[]Edge{
			Edge{0, 1, 1},
			Edge{1, 2, 1},
			Edge{2, 3, 1},
			Edge{3, 0, 1},
			Edge{4, 5, 1},
			Edge{5, 6, 1},
			Edge{7, 8, 1},
			Edge{8, 9, 1},
		}, true)

	comps := gr.ConnectedComponents()

	eulerianMap := make(map[int]bool)
	freeVertexMap := make(map[uint64]bool)

	for i := range comps {
		eulerianMap[i] = true

		for k := range comps[i] {
			if len(gr.VertexEdges[k])%2 == 0 {
				freeVertexMap[k] = true
			} else {
				eulerianMap[i] = false
				freeVertexMap[k] = false
			}
		}
		//comp = gr
		//fmt.Println(i)
	}

	spew.Dump(comps)
	spew.Dump(eulerianMap)
	spew.Dump(freeVertexMap)
	//C.initGalvo()
	fmt.Println("hello world!")
	//C.closeGalvo()
}
