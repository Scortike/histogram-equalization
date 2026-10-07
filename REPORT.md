# REPORT

## Why was the previous multithreading approach with `std::atomic` so extremely slow?

The initial implementation used a shared global histogram updated via `std::atomic`, which led to significant performance degradation because multiple CPU cores were constantly competing to update the same memory locations and had to synchronize these changes with each other. Although thread-safe, this design created a scalability bottleneck in the hot path.

As a result, I redesigned the histogram computation using thread-local histograms. Each thread accumulates results independently in its own local buffer, eliminating synchronization during image processing. A final reduction step then merges all local histograms into a single global histogram. This removes the main source of contention and allows threads to process their image chunks independently, resulting in much better parallel performance.

---

## What memory layout did you choose for the image internal data (AoS vs SoA) and why?

I used a SoA (Structure of Arrays) layout for the image data representation, where each color channel is stored in a separate contiguous array, as it provides more SIMD-friendly memory access patterns compared to AoS. Storing each channel in contiguous memory improves cache locality and enables efficient vectorized loads of *homogeneous* data, which is beneficial for auto-vectorization.

In this specific implementation, the impact of the memory layout on performance is moderate, since the algorithm still performs per-pixel mixed computations, including luminance calculation and LUT indexing, which limits full SIMD utilization.

Nevertheless, SoA was chosen as a more scalable and SIMD-oriented representation, aligning the data layout with potential future optimizations and more vectorized processing stages.

---

## Did you utilize auto-vectorization (SIMD) and how did you verify the compiler generated vector instructions?

The implementation was developed with SIMD-friendly principles. Auto-vectorization was verified using GCC optimization reports generated with `-fopt-info-vec`, which confirmed that the compiler generated vectorized code for several arithmetic code paths.

However, the main image-processing loops could not be effectively auto-vectorized. During histogram construction, each iteration updates a histogram bin determined by the pixel intensity, resulting in data-dependent indexing and irregular memory access patterns. A similar limitation exists during the equalization stage, where the normalized luminance value is obtained through a LUT lookup (`equalized_lum[bin]`). These indirect memory accesses prevent the compiler from transforming the loops into efficient SIMD code.

I experimented with replacing LUT lookups by a precomputed per-pixel cache of equalized values to create a more sequential access pattern. While this improved vectorization opportunities, it introduced an additional full-image pass (`O(W × H)`), whose overhead outweighed the potential SIMD gains. As a result, the final implementation relies primarily on algorithmic and memory-layout optimizations, while SIMD remains limited to smaller arithmetic regions identified by the compiler.
