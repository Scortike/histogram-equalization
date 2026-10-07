# Image Processing Optimization Task

In the `src` folder, you will find a program that opens an image, applies a histogram equalization algorithm, and saves the result. You can run the program by providing the input and output file paths, as well as a reference image path to evaluate mathematical errors:
`> sklm_test input.jpg output.ppm reference.ppm`

## The Problem (What needs to be done)

The current implementation is incredibly slow and contains structural, algorithmic, and hardware-level issues. Your task is to optimize the `Histogram equalization time` phase to **< 20ms** (or at least < 30ms) on modern hardware, while strictly satisfying the following constraints:

1. **Algorithmic Correctness (Color Shift Bug)**: 
   The current algorithm applies histogram equalization to the R, G, and B channels independently. This is fundamentally wrong and results in severe color balance distortion. 
   **Action Required:**
   - First, write a *slow, but mathematically correct* single-threaded implementation of histogram equalization that converts pixels to the `YCbCr` (or `HSV/HSL`) color space, equalizes **only** the luminance/value channel (`Y` or `V`), and converts back to `RGB`.
   - Save the output of this correct, slow function as `reference.ppm`.
   - After that, write a *highly optimized, ultra-fast* implementation of the exact same logic for `output.ppm`. The built-in `compareTwoImages` function in `Test.cpp` will confirm that you haven't broken the math during aggressive optimization. A difference of <= 0.5% per channel is allowed to account for float/integer rounding in YCbCr transforms.

2. **Architectural Optimizations**:
   Previous developers tried to add multithreading using `std::atomic<size_t> g_histogram[256]` and `g_pixelsProcessed`, but the code became EVEN SLOWER. You must understand why this happened, fix it, and explain the root cause.

3. **Memory Constraints**:
   The current `Pixel` struct utilizes `double`s. You are allowed, and strongly encouraged, to completely redesign the internal storage layout in the `Image` class (Data-Oriented Design). However, the CLI interface of the utility must remain unchanged.

4. **A Special Surprise: Bugs and Crashes!**:
   Memory leaks, unclosed file descriptors, and Out-Of-Bounds (UB) array errors have been intentionally introduced into the code. You must find them, fix them, or replace these code sections with modern, safe C++ standards (modern C++ without raw `new`/`delete`). Profiling and memory leak detection tools will be used during evaluation.

5. **Mandatory REPORT.md**:
   Execution of code is only half the task. Your evaluation heavily depends on your ability to explain the underlying processes occurring within the CPU architecture. Create a `REPORT.md` file in the root of the project and briefly describe:
   - Why was the previous multithreading approach with `std::atomic` so extremely slow? (Using industry-standard computer architecture terminology is expected).
   - What memory layout did you choose for the image internal data (AoS vs SoA) and why?
   - Did you utilize auto-vectorization (SIMD) and how did you verify the compiler generated vector instructions?

## AI and Tooling Rules
**Important:** You are completely allowed, and even encouraged, to use ANY tools available to you. This includes ChatGPT, Claude, GitHub Copilot, and absolute autonomous AI agents. However, be extremely careful: relying blindly on AI without understanding the architectural and algorithmic logic will result in a failed assignment.

## Evaluation Criteria
- Execution speed (the faster, the better). A difference of <= 0.5% from your own generated slow YCbCr reference is acceptable.
- Code quality (Modern C++ paradigms, cleanliness, readability, RAII).
- **Most Important**: The depth of hardware and software understanding demonstrated in the `REPORT.md`.

Good luck!