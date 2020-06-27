-- Lucimalir v0.1
-- Example 1
-- Simplest default mandelbrot

-- Creating filter objects
local iv1 = filters.inputs.view()
local fb1 = filters.fractals.basic()
local rb1 = filters.renderers.basic()
local oi1 = filters.outputs.image()

-- By default, images aren't automatically saved
oi1.params.save_files = true

-- Creating the pipeline
iv1:connect_next(fb1)
fb1:connect_next(rb1)
rb1:connect_next(oi1)

-- Starting rendering
iv1:request_rendering()

-- Waiting until everything completes
core.await_completion()
print("Completed")