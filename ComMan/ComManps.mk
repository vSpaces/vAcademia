
ComManps.dll: dlldata.obj ComMan_p.obj ComMan_i.obj
	link /dll /out:ComManps.dll /def:ComManps.def /entry:DllMain dlldata.obj ComMan_p.obj ComMan_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ComManps.dll
	@del ComManps.lib
	@del ComManps.exp
	@del dlldata.obj
	@del ComMan_p.obj
	@del ComMan_i.obj
