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

fb1.params.iterations = 256

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

-- For a 4D vector (X, Y, Z, W)
-- Fractal value is computed by iteration of f(z) = z^2 + c
-- c is (X + Yi), starting z is (Z + Wi)
-- This way we can generalize mandelbrot and julia fractals

-- Basic mandelbrot
iv1.params.view = geom4d.view(geom4d.pos { -0.8, 0.156, 0, 0 }, geom4d.rot(), 3)

oi1.params.base_filename = "ex3_mandelbrot"
iv1:request_rendering()
oi1:await_completion()

-- Rotate the view in XZ and YW planes by 90 degrees, effectively swapping XY with ZW.
-- Produces a Julia fractal
local XY_to_ZW = geom4d.rot.XZ(-math.pi/2) * geom4d.rot.YW(-math.pi/2)
iv1.params.view = iv1.params.view:rotate(XY_to_ZW)

oi1.params.base_filename = "ex3_julia"
iv1:request_rendering()
oi1:await_completion()

-- If we move around the julia plane (ZW), mandelbrot will be deformed
-- because the iteration starts from a nonzero value
-- The deformation of mandelbrot is related to position in julia,
-- similar to  how deformation of julia is related to position in mandelbrot

iv1.params.view = geom4d.view(geom4d.pos { -0.8, 0.156, -0.7, 0.3 }, geom4d.rot(), 3)

oi1.params.base_filename = "ex3_shifted_mandelbrot"
iv1:request_rendering()
oi1:await_completion()

iv1.params.view = iv1.params.view:rotate(XY_to_ZW)

oi1.params.base_filename = "ex3_shifted_julia"
iv1:request_rendering()
oi1:await_completion()

-- We can also rotate the viewing plane by another angle and see how mandelbrot turns into julia
-- You can increase the number of frames to make the animation smoother

local frames = 16
for i = 1, frames do
  local angle = -math.pi / 2 * (i - 1) / (frames - 1)
  iv1.params.view = geom4d.view(geom4d.pos { -0.8, 0.156, 0, 0 }, geom4d.rot.XZ(angle) * geom4d.rot.YW(angle), 3)
  oi1.params.base_filename = "ex3_rotate" .. i
  iv1:request_rendering()
  oi1:await_completion()
end

-- Other rotations

iv1.params.view = geom4d.view(geom4d.pos { -1.3, 0, 0, 0 }, geom4d.rot.XZ(math.pi * 0.25) * geom4d.rot.YW(math.pi * 0.25) * geom4d.rot.ZW(math.pi * 1.5), 2)
oi1.params.base_filename = "ex3_other_rotation1"
iv1:request_rendering()
oi1:await_completion()

iv1.params.view = geom4d.view(geom4d.pos { -1.3, 0, 0, 0 }, geom4d.rot.XW(math.pi * 0.25) * geom4d.rot.YZ(math.pi * 0.25) * geom4d.rot.ZW(math.pi * 1.5), 2)
oi1.params.base_filename = "ex3_other_rotation1"
iv1:request_rendering()
oi1:await_completion()

-- We can also assign a random transformation matrix instead of a proper rotation matrix

local images = 16
for i = 1, images do
  local matrix = {}
  for j = 1, 16 do
    matrix[j] = -2 + 4 * math.random()
  end
  iv1.params.view = geom4d.view(geom4d.pos { -1.3, 0.3, 0, 0 }, geom4d.rot(matrix), 2)
  oi1.params.base_filename = "ex3_random" .. i
  iv1:request_rendering()
  oi1:await_completion()
end