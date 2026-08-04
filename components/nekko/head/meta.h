struct AppVer{
  short maj;
  short min;
  short fix;

  const char *codename;
  const char *gccver;
  const char *date;
};

extern const AppVer c_assemble_appver;
