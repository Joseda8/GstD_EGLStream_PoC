consumer_src = [
  '../Ancillary/fd_send.c',
  'consumer.c'
]

# Create applications
exe_name = '@0@-@1@'.format('consumer', project_version)
executable(exe_name,
  [consumer_src, common_src],
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_dep, egl_dep, opengl_dep, x11_dep],
  c_args: c_args,
)
