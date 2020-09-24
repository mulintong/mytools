call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x86  
   
cd boost_1_64_0
call bootstrap.bat
   
rem Most libraries can be static libs  
b2 -j8 toolset=msvc-14.0 address-model=32 architecture=x86 link=static threading=multi runtime-link=shared --build-type=minimal stage --stagedir=stage/win32 
b2 -j8 toolset=msvc-14.0 address-model=32 architecture=x86 link=static threading=multi runtime-link=static --build-type=minimal stage --stagedir=stage/win32 
   
pause