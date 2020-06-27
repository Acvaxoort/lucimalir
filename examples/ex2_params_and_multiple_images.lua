-- Lucimalir v0.1
-- Example 2
-- Simple parametrization, rendering multiple images

local iv1 = filters.inputs.view()
local fb1 = filters.fractals.basic()
local rb1 = filters.renderers.basic()
local oi1 = filters.outputs.image()
local rb2 = filters.renderers.basic()
local oi2 = filters.outputs.image()
oi1.params.save_files = true
oi2.params.save_files = true

local width = 3840
local height = 2160

-- First rendering, slight zoom in, inverted coloring

-- Centered around (-0.65, -0.45), scale = 0.15 (zoomed in)
iv1.params.view = geom4d.view(geom4d.pos { -0.65, 0.45, 0, 0 }, geom4d.rot(), 0.15)

-- More iterations
fb1.params.iterations = 256

-- Inverted coloring
-- par1, par2, par3 are R, G, B
-- low is the value for the lowest value in fractal (escape in 0 iterations)
-- high is the value for the highest value in fractal (no escape)
rb1.params.low_par1 = 1
rb1.params.low_par2 = 1
rb1.params.low_par3 = 1
rb1.params.high_par1 = 0
rb1.params.high_par2 = 0
rb1.params.high_par3 = 0

-- Changing filename
oi1.params.base_filename = "ex2_mandelbrot1"

-- Changing resolution, you could change resolution of rb1 but it would be
-- overwritten by ot1's parameter upon connecting
oi1.params.width = width
oi1.params.height = height

-- Second color scheme
-- par1, par2, par3 are H, S, V
rb2.params.color_model = "HSV"
rb2.params.low_par1 = math.pi
rb2.params.low_par2 = 0
rb2.params.low_par3 = 1
rb2.params.high_par1 = math.pi * 2
rb2.params.high_par2 = 1
rb2.params.high_par3 = 0

-- Changing filename and resolution, similar to oi1
oi2.params.base_filename = "ex2_mandelbrot2"
oi2.params.width = width
oi2.params.height = height


iv1:connect_next(fb1)
fb1:connect_next(rb1)
rb1:connect_next(oi1)

-- The order of connection matters, we first need to connect rb2 with oi2
-- in order to propagate out resolution change, otherwise connection between
-- fb1 and rb2 would fail because rb1 wants 3840x2160 but rb2 wants 1920x1080
rb2:connect_next(oi2)
fb1:connect_next(rb2)

iv1:request_rendering()

-- Await completion but only on the image filter. The difference is that
-- This way, next rendering doesn't have to wait until image is compressed
-- into png, which takes a relatively long time-
oi1:await_completion()
oi2:await_completion()
print("Completed rendering 1")


-- Second rendering, a different region in mandelbrot, more zoom, more iterations
iv1.params.view = geom4d.view(geom4d.pos { -1.354696, -0.06813, 0, 0 }, geom4d.rot(), 0.000005)
fb1.params.iterations = 4096

iv1:request_rendering()
rb1.params.input_gamma = 0.5
rb1.params.output_gamma = 0.4
rb2.params.input_gamma = 0.5
rb2.params.output_gamma = 0.4

-- If you run the program with an interactive shell, the prompt will appear
-- after rendering is completed but before images are saved
oi1:await_completion()
oi2:await_completion()
print("Completed rendering 2")