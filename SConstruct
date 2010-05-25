import os,sys
conf		= {}
mcu			= "atmega644"
conf['MCU']	= mcu
conf['F_CPU'] = 1000000

crosspack	= '/usr/local/CrossPack-AVR/bin'

if(None != FindFile('tty.PL2303-00001224',['/dev/'])):
	programmer = "-c stk500 -P /dev/tty.PL2303-00* -B 8.68"
else:
	programmer = "-c avrisp2 -P usb -B 8.68"

avr	 = Environment(
				tools=['default','avr'],toolpath=['scons'],
				MCU=mcu,AVRPATH=crosspack)
dude	= Environment(
				tools=['avrdude'],toolpath=['scons'],
				MCU=mcu,PROGRAMMER=programmer,AVRPATH=crosspack)

avr.Append(CPPPATH=['.','avrlib'])
avr.Append(CFLAGS=['-Os','-Wall','-std=gnu99','-funsigned-char','-funsigned-bitfields','-fshort-enums','-fpack-struct'])

### Procyon ##################################################################
# net_files	   = [os.path.join('net', f) for f in Split('arp.c dhcp.c icmp.c ip.c net.c netstack.c')]

general_files   = [os.path.join('avrlib',f) for f in Split('spi.c mmc.c rprintf.c')]
# # general_files.append('uart2.c')
# 
procyon = avr.Library('procyon', general_files)
# avr.DisplaySizes(procyon,TOTALS=True)

##############################################################################
def generate_from_template(target,source,env):
	assert(len(source)==1)
	assert(len(target)>=1)
	from string import Template
	f = open(str(source[0]),'r')
	t = Template(f.read())
	f.close()
	f = open(str(target[0]),'w')
	f.write(t.substitute(conf))
	f.flush()
	f.close()
avr.Append(BUILDERS={
	'GenerateFromTemplate': Builder(action=generate_from_template,src_suffix='.template')})


global_header = avr.GenerateFromTemplate('global.h.template')
Depends(global_header, 'SConstruct')


target = avr.Program(Split('main.c uart.c lm74.c'),LIBS=['procyon'],LIBPATH=['.'])
Depends(target,procyon)
avr.DisplaySizes('size',target)

hex_file = avr.HexFile(target)
avr.EepFile(target)
avr.DisplayASM('asm',target)

Default(hex_file)

if sys.platform != 'win32':
	dude.ReadFuses()
	dude.Erase()
	dude.Flash(hex_file)

if GetOption('clean'):
	Default([hex_file,target])
