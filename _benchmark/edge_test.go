package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/effect"
	"github.com/octu0/blurry"
)

func BenchmarkEdge(b *testing.B) {
	b.Run("bild/EdgeDetection", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = effect.EdgeDetection(testImg, 1.0)
		}
	})
	b.Run("blurry/Edge", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Edge(testImg)
		}
	})
}
