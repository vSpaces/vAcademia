#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

static const char gpszUIDefault[]=""
"<?xml version=\"1.0\" encoding=\"Windows-1251\"?>"
"<rmml>\n"
"  <scene _name=\"UI\">\n"
"    <text _name=\"txProgress\" _x=\"10\" _y=\"10\" _width=\"50\" value=\"0%\"/>\n"
"    <sequencer _name=\"seqProgress\" _playing=\"true\" _loop=\"true\">\n"
"      <script>\n"
"//        trace('Player.module.loadingProgress:'+Player.module.loadingProgress);\n"
"        if(Player.mode==Player.MODE_LOADING){\n"
"          txProgress._visible=true;\n"
"          txProgress.value='<![CDATA[<font size=\"20\" color=\"#FFFFFF\">'+Math.round(Player.module.loadingProgress*100)+'%</font>]]>';\n"
"        }else{\n"
"          txProgress._visible=false;\n"
"          seqProgress._playing=false;\n"
"        }\n"
"        true;\n"
"      </script>\n"
"    </sequencer>\n"
"  </scene>\n"
"</rmml>\n"
;

}
