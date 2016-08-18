
Cscl3DWSps.dll: dlldata.obj Cscl3DWS_p.obj Cscl3DWS_i.obj
	link /dll /out:Cscl3DWSps.dll /def:Cscl3DWSps.def /entry:DllMain dlldata.obj Cscl3DWS_p.obj Cscl3DWS_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del Cscl3DWSps.dll
	@del Cscl3DWSps.lib
	@del Cscl3DWSps.exp
	@del dlldata.obj
	@del Cscl3DWS_p.obj
	@del Cscl3DWS_i.obj
