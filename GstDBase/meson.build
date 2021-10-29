gstd_base_src = [
  'gstd.c'
]

# Create applications
exe_name = '@0@-@1@'.format('gstd_base', project_version)
executable(exe_name,
  [gstd_base_src, common_src],
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_dep, egl_dep, opengl_dep, x11_dep],
  c_args: c_args,
)
