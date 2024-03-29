# Build against libxml2 from Rtools. This is not be used on CRAN (see Makevars.win)
if (!file.exists("../windows/libxml2-2.9.10/include/libxml2/libxml/parser.h") && Sys.getenv("LIB_XML") == "") {
  if(getRversion() < "3.3.0") setInternet2()
  download.file("https://github.com/rwinlib/libxml2/archive/v2.9.10.zip", "lib.zip", quiet = TRUE)
  dir.create("../windows", showWarnings = FALSE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
}
