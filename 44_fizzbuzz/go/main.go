package main

import (
	"flag"
	"fmt"
)

func main() {

	limit := flag.Int("limit", 0, "Upper loop limit")
	flag.Parse()

	for i := 0; i < *limit; i++ {
		if i%15 == 0 {
			fmt.Printf("fizzbuzz\n")
		} else if i%5 == 0 {
			fmt.Printf("buzz\n")
		} else if i%3 == 0 {
			fmt.Printf("fizz\n")
		} else {
			fmt.Printf("%d\n", i)
		}
	}
}
