local param_type = "classes.param"

local filter_type = "classes.filter"

local param_width = {
  type = "number",
  desc = "image width in pixels",
  default = 1920,
}

local param_height = {
  type = "number",
  desc = "image height in pixels",
  default = 1080,
}

local param_num_layers = {
  type = "number",
  desc = "number of layers of the image",
  default = 1,
}

local param_precision = {
  type = "number, either 32 or 64",
  desc = "Floating point precision, 32 bits or 64 bits",
  default = "64",
}

local help = {
  label = "Help",
  desc = "Help root, type help.object to get help about object. The objects are available in global namespace with exactly the same structure as inside help (help.filters.fractals.basic describes filters.fractals.basic). The \"classes\" section is an exception, the table doesn't actually exist but help.classes provides information about what properties and methods objects have. Use \"lasthelp\" to access last visited help page. Use \"lastparent\" to access parent of last visited page. Use some_help_page.parent to navigate to its parent page",
  filters = {
    label = "Filters",
    desc = "Filters are the primary building block of the rendering pipeline, functions in this namespace return filter objects, see help.classes.filter",
    fractals = {
      label = "Fractal rendering filters",
      basic = {
        label = "Basic fractal renderer",
        desc = "Basic renderer; 4D generalization of Mandelbrot and Julia; Renders Mandelbrot on (x, y) and Julia on (z, w)",
        args = {},
        returns = filter_type,
        parameters = {
          precision = param_precision,
          iterations = {
            type = param_type,
            desc = "Maximum number of iterations of the fractal equation",
            default = "constant(64)",
          },
        },
      },
    },
    renderers = {
      label = "Fractal data to image filters",
      basic = {
        label = "Basic fractal data renderer",
        desc = "Converts integer matrix (fractal renderer output) to color image",
        args = {},
        returns = filter_type,
        parameters = {
          width = param_width,
          height = param_height,
          num_layers = param_num_layers,
          low_par1 = {
            type = param_type,
            desc = "Value of first color parameter for lowest value in fractal",
            default = "constant(0)",
          },
          low_par2 = {
            type = param_type,
            desc = "Value of second color parameter for lowest value in fractal",
            default = "constant(0)",
          },
          low_par3 = {
            type = param_type,
            desc = "Value of third color parameter for lowest value in fractal",
            default = "constant(0)",
          },
          high_par1 = {
            type = param_type,
            desc = "Value of first color parameter for highest value in fractal",
            default = "constant(1)",
          },
          high_par2 = {
            type = param_type,
            desc = "Value of second color parameter for highest value in fractal",
            default = "constant(1)",
          },
          high_par3 = {
            type = param_type,
            desc = "Value of third color parameter for highest value in fractal",
            default = "constant(1)",
          },
          input_gamma = {
            type = param_type,
            desc = "Gamma coefficient for input correction",
            default = "constant(1)",
          },
          output_gamma = {
            type = param_type,
            desc = "Gamma coefficient for output correction",
            default = "constant(1)",
          },
          color_model = {
            type = "string / one of color_model",
            desc = "Color model, how parameters are treated",
            default = "\"RGB\"",
          },
        },
        color_model = {
          label = "Basic fractal data renderer color model",
          enum_values = {
            RGB = {
              desc = "Parameters: red, green, blue; all in range (0, 1)"
            },
            HSV = {
              desc = "Parameters: hue, saturation, value; hue in radians, full rotation is one cycle around the color wheel, others in range (0, 1)"
            },
          },
        },
      },
    },
    images = {
      label = "Image manipulation filters",
      merge = {
        label = "Image merge",
        desc = "Combines multiple images into one",
        args = {},
        returns = filter_type,
        parameters = {
          width = param_width,
          height = param_height,
          num_layers = param_num_layers,
          mode = {
            type = "string / one of mode",
            desc = "How the images are combined",
            default = "\"ADD_LIMIT\"",
          },
          multiplier = {
            type = param_type,
            desc = "Multipliers of individual layers before the merging",
            default = "constant(1)",
          },
          post_multiplier = {
            type = "number",
            desc = "Multiplier of output after first step of merging, after tanh and after limit, see mode description",
            default = 1,
          },
        },
        mode = {
          label = "Image merge mode enum",
          enum_values = {
            ADD_LIMIT = {
              desc = "Sum all images and cap using limiter/saturation",
            },
            ADD_TANH = {
              desc = "Sum all images and cap using tanh",
            },
            ADD_TANH_LIMIT = {
              desc = "Sum all images and cap using tanh, apply post_multiplier and cap using limiter/saturation",
            },
            SUB_LIMIT = {
              desc = "Inverse ADD_LIMIT of inverses of all images (subtractive mixing)",
            },
            SUB_TANH = {
              desc = "Inverse ADD_TANH of inverses of all images (subtractive mixing)",
            },
            SUB_TANH_LIMIT = {
              desc = "Inverse ADD_TANH_LIMIT of inverses of all images (subtractive mixing)",
            },
            MULTIPLY = {
              desc = "Multiply all images"
            },
            MULTIPLY_SUB = {
              desc = "Inverse of multiplication of inverses of all images"
            },
          },
        },
      },
    },
    inputs = {
      label = "External inputs",
      view = {
        label = "View input",
        desc = "Passes given rendering view to the buffer",
        args = {},
        returns = filter_type,
        parameters = {
          precision = param_precision,
          view = {
            type = "view4d (see help.geom4d.view4d)",
            desc = "Desired view",
            default = "classes.geom4d.view",
          },
        },
      },
    },
    outputs = {
      label = "External outputs",
      image = {
        label = "Image output",
        desc = "Ends the rendering pipeline, can save images to file or render them to the screen",
        args = {},
        returns = filter_type,
        parameters = {
          width = param_width,
          height = param_height,
          num_layers = param_num_layers,
          save_files = {
            type = "boolean",
            desc = "Whether a file is saved on each pass",
            default = "true",
          },
          base_filename = {
            type = "string",
            desc = "Base filename of saved files, numbers of layers and unique number are also added",
            default = "image",
          }
        },
      },
    },
  },
  params = {
    label = "Variable filter parameters",
    desc = "Objects that represent filters that can be different over a range of layers",
    constant = {
      label = "Constant parameter",
      desc = "Value is constant for all layers, usage: constant(value)",
      args = {
        {
          name = "v",
          type = "number",
          desc = "Value of the parameter",
        },
      },
      returns = param_type,
    },
    range = {
      label = "Range parameter",
      desc = "Value is linear interpolation from v1 to v2 or average of v1 and v2 if there is one layer, usage: range(v1, v2)",
      args = {
        {
          name = "v1",
          type = "number",
          desc = "Value for the lower end",
        },
        {
          name = "v2",
          type = "number",
          desc = "Value for the higher end",
        },
      },
      returns = param_type,
    },
    range_exp = {
      label = "Exponential range parameter",
      desc = "Value is linear interpolation on exponential scale from v1 to v2 or average of v1 and v2 if there is one layer, usage: range_exp(v1, v2)",
      args = {
        {
          name = "v1",
          type = "number",
          desc = "Value for the lower end",
        },
        {
          name = "v2",
          type = "number",
          desc = "Value for the higher end",
        },
      },
      returns = param_type,
    },
    array = {
      label = "Array parameter",
      desc = "Value is taken from array at given position if length of array is equal to the number of layers; if it isn't equal, some values are repeated or skipped to match requested length",
      args = {
        {
          name = "array",
          type = "table (indices 1, 2, ...)",
          desc = "Values of the parameter",
        },
      },
      returns = param_type,
    }
  },
  geom4d = {
    label = "4D view manipulation",
    pos = {
      label = "Create 4D vector (position)",
      desc = "Creates a 4D vector",
      args = {
        {
          name = "array",
          type = "table (indices 1 to 4)",
          desc = "(X, Y, Z, W)",
          default = "{0, 0, 0, 0}",
        },
      },
      returns = "classes.geom4d.pos",
    },
    rot = {
      label = "Create 4x4 matrix (4D rotation / transformation matrix)",
      desc = "Creates a 4x4 matrix",
      args = {
        {
          name = "array",
          type = "table (indices 1 to 16)",
          desc = "16 values of the matrix, row by row",
          default = "{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} (identity matrix)",
        },
      },
      returns = "classes.geom4d.rot",
      XY = {
        label = "Rotation of XY plane",
        desc = "Creates a transformation matrix for rotation of XY plane",
        args = {
          {
            name = "angle",
            type = "number",
            desc = "Angle in radians",
          },
        },
      },
      YZ = {
        label = "Rotation of YZ plane",
        desc = "Creates a transformation matrix for rotation of YZ plane",
        args = {
          {
            name = "angle",
            type = "number",
            desc = "Angle in radians",
          },
        },
      },
      ZW = {
        label = "Rotation of ZW plane",
        desc = "Creates a transformation matrix for rotation of ZW plane",
        args = {
          {
            name = "angle",
            type = "number",
            desc = "Angle in radians",
          },
        },
      },
      XZ = {
        label = "Rotation of XZ plane",
        desc = "Creates a transformation matrix for rotation of XZ plane",
        args = {
          {
            name = "angle",
            type = "number",
            desc = "Angle in radians",
          },
        },
      },
      YW = {
        label = "Rotation of YW plane",
        desc = "Creates a transformation matrix for rotation of YW plane",
        args = {
          {
            name = "angle",
            type = "number",
            desc = "Angle in radians",
          },
        },
      },
      XW = {
        label = "Rotation of XW plane",
        desc = "Creates a transformation matrix for rotation of XW plane",
        args = {
          {
            name = "angle",
            type = "number",
            desc = "Angle in radians",
          },
        },
      },
    },
    view = {
      label = "Create 4D view",
      desc = "Creates a 4D view",
      args = {
        {
          name = "position",
          type = "classes.geom4d.pos",
          desc = "position of the center",
          default = "geom4d.pos()",
        },
        {
          name = "rotation",
          type = "classes.geom4d.rot",
          desc = "rotation / transformation matrix",
          default = "geom4d.rot()",
        },
        {
          name = "scale",
          type = "number",
          desc = "how wide will the view span if the image was square, scale 4 around the origin point means range (-2, 2)",
          default = "4",
        },
      },
      returns = "classes.geom4d.view",
    },
  },
  core = {
    label = "Core",
    desc = "Object that aggregates all created filters automatically and is responsible for scheduling the rendering.",
    num_threads = {
      label = "Number of threads",
      type = "number",
      desc = "Number of working threads that compute/render all filters. If set to 0 or lower, a default number equal to number of threads in the CPU is used. Maximum 256.",
      read_only = false,
      default = "number of threads in the CPU",
    },
    filters = {
      label = "Filters",
      type = "table of classes.filter",
      desc = "Returns a table containing filter objects/proxies to all filters.",
      read_only = true,
    },
    external_input_filters = {
      label = "External input filters",
      type = "table of classes.filter",
      desc = "Returns a table containing filter objects/proxies to all filters that are external inputs.",
      read_only = true,
    },
    external_output_filters = {
      label = "External output filters",
      type = "table of classes.filter",
      desc = "Returns a table containing filter objects/proxies to all filters that are external outputs.",
      read_only = true,
    },
    filter_group_roots = {
      label = "Filter group roots",
      type = "table of classes.filter",
      desc = "Returns a table containing filter objects/proxies to all filters that are connected to at least one filter.",
      read_only = true,
    },
    pipeline_roots = {
      label = "Pipeline roots",
      type = "table of classes.filter",
      desc = "Returns a table containing filter objects/proxies to all filters that begin a rendering pipeline (where output filter exists).",
      read_only = true,
    },
    stray_filters = {
      label = "Stray filters",
      type = "table of classes.filter",
      desc = "Returns a table containing filter objects/proxies to all filters that aren't connected to anything.",
      read_only = true,
    },
    await_completion = {
      label = "Await completion",
      desc = "Blocks execution until everything that was requested to compute was computed",
      args = {},
    },
  },
  classes = {
    label = "Classes",
    filter = {
      label = "Filter",
      desc = "Main building block of the rendering pipeline. Rendering will be possible if they are connected with each other into a full chain, with external input in the beginning and one or more external outputs at the end. When connecting filters, a buffer is created between them if no buffer was present. One filter/buffer can have multiple outputs but only one input.  The filter object is only a proxy object to an actually filter object in the core. See help.core for more details.",
      params = {
        label = "Parameters",
        desc = "Table that holds parameters. All parameters are read/write properties. Changing any parameter will result in a rendering request if auto_request is true. Some parameters influence the kind of input the filter can take (for example image resolution) and will propagate these changes to the previous filter. Sometimes the propagation can fail if the previous filter is connected to another output whose parameters conflict with the changes. To see what parameters are available for a particular filter type, see \"Parameters\" section in help.filters.<filter type>. ";
        type = "table",
        read_only = true,
      },
      auto_request = {
        label = "Auto request",
        desc = "Whether the filter automatically starts rendering if its parameters are changed. This is more useful for interactive fractal display than scripts.",
        read_only = false,
        type = "boolean",
        default = "false",
      },
      input = {
        label = "Input",
        desc = "Filter's input buffer. It is writable but only nil assignment is supported. If you disconnect input from the filter, the buffer will stay connected to the other filters connected to it.",
        read_only = false,
        type = "classes.buffer_collection",
        default = "nil",
      },
      output = {
        label = "Output",
        desc = "Filter's output buffer. It is writable but only nil assignment is supported If you disconnect output from the filter, the buffer will stay connected to the other filters connected to it.",
        read_only = false,
        type = "classes.buffer_collection",
        default = "nil",
      },
      expired = {
        label = "Expired",
        desc = "Whether the filter exists in core. Expired filter object is no longer useful and should be forgotten.",
        read_only = true,
        type = "boolean",
        default = "false",
      },
      type = {
        label = "Type",
        desc = "Exact type of the filter, equivalent to path of the function inside filters namespace that creates the filter (e.g. fractals.basic)",
        read_only = true,
        type = "string",
      },
      interrupt = {
        label = "Interrupt",
        desc = "Stops rendering/computing on this filter",
        args = {},
      },
      request_rendering = {
        label = "Request rendering",
        desc = "Request rendering on a filter. Note that rendering is automatically started if a preceeding filter completes its job or a part of its job (depending on type of the filter).",
        args = {},
      },
      connect_previous = {
        label = "Connect previous",
        desc = "Connects this filter's input to provided filter, if the operation is succesful, the provided filter becomes previous of this filter in pipeline. If this filter has an input buffer and previous filter has an output buffer, the operation will fail. Not every two filters are compatible and connecting them can result in an error.",
        args = {
          {
            name = "previous",
            type = "classes.filter",
            desc = "filter to be connected",
          },
        },
      },
      connect_next = {
        label = "Connect next",
        desc = "Connects this filter's output to provided filter, if the operation is succesful, the provided filter becomes next of this filter in pipeline. If this filter has an output buffer and next filter has an input buffer, the operation will fail. Not every two filters are compatible and connecting them can result in an error.",
        args = {
          {
            name = "next",
            type = "classes.filter",
            desc = "filter to be connected",
          },
        },
      },
      remove = {
        label = "Remove",
        desc = "Removes the filter from the core, which results in disconnecting it from any filter/buffer it was connected to and interrupting all rendering.",
        args = {},
      },
      await_completion = {
        label = "Await completion",
        desc = "Blocks the execution until the filter completes its job if the filter type is outputs.image.",
        args = {},
      }
    },
    buffer_collection = {
      label = "Buffer collection",
      desc = "Not implemented",
    },
    buffer = {
      label = "Buffer",
      desc = "Not implemented",
    },
    param = {
      label = "Filter variable parameter",
      desc = "Parameters that can have different values for various layers of buffers.",
    },
    geom4d = {
      label = "4D view manipulation",
      desc = "Objects for manipulating 4D views. Note that all of the objects are immutable.",
      pos = {
        label = "4D vector (position)",
        desc = "Vector describing a position in 4D space. Supports addition with other vectors",
        values = {
          label = "Values",
          desc = "Returns the vector in table form";
          type = "table",
          read_only = true,
        },
        translate = {
          label = "Translate",
          desc = "Translates the vector by the other vector, equivalent to + operator",
          args = {
            name = "v",
            type = "classes.geom4d.pos",
            desc = "The other vector",
          },
        },
      },
      rot = {
        label = "4x4 matrix (4D rotation / transformation matrix)",
        desc = "Matrix describing a transformation of points in 4D space. While provided functions always return rotation matrices (orthogonal with determinant = 1), any transformation matrix can be created. Note that rotations happen along planes, not axes.",
        values = {
          label = "Values",
          desc = "Returns the matrix in table form, row by row";
          type = "table",
          read_only = true,
        },
        rotate = {
          label = "Rotate",
          desc = "Multiplies the matrix with the other (rotates it by its rotation), equivalent to * operator",
          args = {
            name = "v",
            type = "classes.geom4d.rot",
            desc = "The other matrix",
          },
        },
      },
      view = {
        label = "4D view",
        desc = "Describes a view in 4D space of the fractals, defines what 4D position each pixel of image has. Position describes the center of the image. Rotation describes a matrix transformation of the values. Scale describes how wine the image spans in the 4D space (scale of 4 with pos in origin and no rotation means (-2,2) in x and y).",
        pos = {
          label = "Position",
          desc = "Position of the view";
          type = "classes.geom4d.pos",
          read_only = true,
        },
        rot = {
          label = "Rotation",
          desc = "Rotation of the view";
          type = "classes.geom4d.rot",
          read_only = true,
        },
        scale = {
          label = "Scale",
          desc = "Scale/span of the view";
          type = "number",
          read_only = true,
        },
        set_position = {
          label = "Set position",
          desc = "Sets position to the provided vector, returns new object because the objects are immutable",
          args = {
            name = "pos",
            type = "classes.geom4d.pos",
            desc = "New position",
          },
        },
        set_rotation = {
          label = "Set position",
          desc = "Sets rotation to the provided matrix, returns new object because the objects are immutable",
          args = {
            name = "rot",
            type = "classes.geom4d.rot",
            desc = "New rotation",
          },
        },
        set_scale = {
          label = "Set scale",
          desc = "Sets scale to the provided number, returns new object because the objects are immutable",
          args = {
            name = "scale",
            type = "number",
            desc = "New scale",
          },
        },
        translate = {
          label = "Translate",
          desc = "Translates position by the provided vector, returns new object because the objects are immutable",
          args = {
            name = "pos",
            type = "classes.geom4d.pos",
            desc = "Position to translate by",
          },
        },
        rotate = {
          label = "Rotate",
          desc = "Rotates rotation by the provided matrix, returns new object because the objects are immutable",
          args = {
            name = "rot",
            type = "classes.geom4d.ret",
            desc = "Rotation to rotate by",
          },
        },
        rescale = {
          label = "Rescale",
          desc = "Changes scale by the provided number (multiplication), returns new object because the objects are immutable",
          args = {
            name = "scale",
            type = "classes.geom4d.scale",
            desc = "Scale to rescale by",
          },
        },
      },
    },
  },
}

return help