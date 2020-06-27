-- Lucimalir v0.1
-- Example 3
-- Generalized 4D space of Mandelbrot and Julia fractals

local iv1 = filters.inputs.view()
local fb1 = filters.fractals.basic()
local rb1 = filters.renderers.basic()
local oi1 = filters.outputs.image()
oi1.params.save_files = true

local width = 1920
local height = 1080

fb1.params.iterations = 4096

rb1.params.color_model = "HSV"
rb1.params.low_par1 = math.pi
rb1.params.low_par2 = 0
rb1.params.low_par3 = 1
rb1.params.high_par1 = math.pi * 2
rb1.params.high_par2 = 1
rb1.params.high_par3 = 0

oi1.params.width = width
oi1.params.height = height

iv1:connect_next(fb1)
fb1:connect_next(rb1)
rb1:connect_next(oi1)

-- Mandelbrot from example 2
iv1.params.view = geom4d.view(geom4d.pos { -1.354696, -0.06813, 0, 0 }, geom4d.rot(), 0.000005)
oi1.params.base_filename = "ex4_64bit"
iv1:request_rendering()
oi1:await_completion()

-- Changing the floating type to 32bit float
fb1.params.precision = 32
oi1.params.base_filename = "ex4_32bit"
iv1:request_rendering()
oi1:await_completion()