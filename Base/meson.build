base_src = [
  'base.c'
]

# Create applications
exe_name = '@0@-@1@'.format('base', project_version)
executable(exe_name,
  [base_src, common_src],
  install: true,
  include_directories : [configinc],
  dependencies : [egl_dep, opengl_dep, x11_dep],
  c_args: c_args,
)
