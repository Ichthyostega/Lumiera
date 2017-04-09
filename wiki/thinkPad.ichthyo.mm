<map version="1.0.1">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node BACKGROUND_COLOR="#6666ff" CREATED="1434127882200" ID="ID_1452170048" MODIFIED="1488486745574" TEXT="Lumi">
<font NAME="SansSerif" SIZE="25"/>
<node CREATED="1434128046296" ID="ID_1900827283" MODIFIED="1448669775410" POSITION="right" TEXT="GUI">
<font NAME="SansSerif" SIZE="14"/>
<node CREATED="1434128054470" HGAP="7" ID="ID_1166611516" MODIFIED="1476376907132" TEXT="Workflow" VSHIFT="-15">
<node CREATED="1455289597596" ID="ID_970065036" MODIFIED="1455289601196" TEXT="work site"/>
<node CREATED="1455289466261" ID="ID_273679080" MODIFIED="1455289469961" TEXT="PresentationState"/>
<node CREATED="1487210691163" ID="ID_1259700796" MODIFIED="1487270046357" TEXT="konzeptioneller Rahmen">
<linktarget COLOR="#34dbd3" DESTINATION="ID_1259700796" ENDARROW="Default" ENDINCLINATION="450;232;" ID="Arrow_ID_114106295" SOURCE="ID_1430752735" STARTARROW="None" STARTINCLINATION="-1346;-1090;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="14"/>
<node CREATED="1487210978300" ID="ID_1502972928" MODIFIED="1487210984415" TEXT="Spot == wo wir sind"/>
<node CREATED="1487211011447" ID="ID_61564732" MODIFIED="1487211040824" TEXT="Work Site == wo wir sein k&#xf6;nnen"/>
<node CREATED="1487211200966" ID="ID_1565474518" MODIFIED="1487211238989" TEXT="Focus == Realisierung des Spot in einem ControlSystem"/>
<node CREATED="1487211243656" ID="ID_953924547" MODIFIED="1487211262209" TEXT="ControlSystem == Steuerungs-Paradigma">
<node CREATED="1487211263461" ID="ID_374086294" MODIFIED="1487211315754" TEXT="Maus / current element"/>
<node CREATED="1487211266901" ID="ID_306043379" MODIFIED="1487211279343" TEXT="Keyboard / Input Focus"/>
</node>
<node CREATED="1487211777687" ID="ID_577769257" MODIFIED="1487211824509" TEXT="Focus Path == kanonische Koordinaten innerhalb von einem UI Frame"/>
<node CREATED="1487211825857" ID="ID_883246945" MODIFIED="1487211951739" TEXT="UI Frame == top-level-Segment (konkret: WorkspaceWindow)"/>
<node CREATED="1487212066024" ID="ID_378199749" MODIFIED="1487212081762" TEXT="Spot Locator == das was navigiert wird, wenn sich der Spot bewegt"/>
</node>
</node>
<node CREATED="1477784761581" HGAP="72" ID="ID_854334914" MODIFIED="1477784783508" TEXT="Rahmen" VSHIFT="-21">
<node CREATED="1481320580216" ID="ID_1418071897" MODIFIED="1481320587160" TEXT="vorantreiben">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1481320590214" ID="ID_635303062" MODIFIED="1481320737392" TEXT="DummySessionConnection">
<arrowlink COLOR="#f9b709" DESTINATION="ID_1343270939" ENDARROW="Default" ENDINCLINATION="-30;-35;" ID="Arrow_ID_1867016690" STARTARROW="None" STARTINCLINATION="-34;-43;"/>
<icon BUILTIN="idea"/>
<node CREATED="1481320639544" ID="ID_1440497947" MODIFIED="1481320641675" TEXT="Singleton"/>
<node CREATED="1481320642455" ID="ID_1938045938" MODIFIED="1481320649996" TEXT="hat fake Session content"/>
<node CREATED="1481320650486" ID="ID_155238526" MODIFIED="1481320661888" TEXT="bietet fake Commands"/>
<node CREATED="1481320666196" ID="ID_1580591981" MODIFIED="1481320675247" TEXT="plus zugeh&#xf6;rige Diagnostik"/>
</node>
</node>
<node CREATED="1477784793993" ID="ID_1868522177" MODIFIED="1487464677579" TEXT="Umbau">
<linktarget COLOR="#bc4930" DESTINATION="ID_1868522177" ENDARROW="Default" ENDINCLINATION="221;282;" ID="Arrow_ID_600425644" SOURCE="ID_707273464" STARTARROW="None" STARTINCLINATION="1925;196;"/>
<icon BUILTIN="hourglass"/>
<node CREATED="1477784813895" ID="ID_1343270939" MODIFIED="1483920891840" TEXT="Model durch UI-Bus ersetzen">
<linktarget COLOR="#f9b709" DESTINATION="ID_1343270939" ENDARROW="Default" ENDINCLINATION="-30;-35;" ID="Arrow_ID_1867016690" SOURCE="ID_635303062" STARTARROW="None" STARTINCLINATION="-34;-43;"/>
<linktarget COLOR="#a9b4c1" DESTINATION="ID_1343270939" ENDARROW="Default" ENDINCLINATION="-197;638;" ID="Arrow_ID_1686519160" SOURCE="ID_202253849" STARTARROW="Default" STARTINCLINATION="-83;-1020;"/>
<node COLOR="#338800" CREATED="1481320683706" ID="ID_1226263415" MODIFIED="1481331062143" TEXT="kl&#xe4;ren, wo der Bus aufgeh&#xe4;ngt wird">
<icon BUILTIN="button_ok"/>
<node CREATED="1481331063782" ID="ID_1606852933" MODIFIED="1481333728957" TEXT="Lifecycle-Frontend UI-Bus"/>
<node CREATED="1481331150275" ID="ID_814186655" MODIFIED="1481333614213" TEXT="CoreService als PImpl"/>
<node CREATED="1481333616753" ID="ID_709873899" MODIFIED="1481333628171" TEXT="Nexus ist Teil von CoreService"/>
<node CREATED="1481335769192" ID="ID_1335469664" MODIFIED="1481335780355" TEXT="Interface-Manager gleicherma&#xdf;en"/>
</node>
<node COLOR="#338800" CREATED="1481320693912" ID="ID_918908848" MODIFIED="1481338438364" TEXT="Lebenszyklus kl&#xe4;ren">
<icon BUILTIN="button_ok"/>
<node CREATED="1481332855167" FOLDED="true" ID="ID_362694314" MODIFIED="1488423342657">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#cf1445">AUA</font>: Henne oder Ei?
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1481332888362" ID="ID_85978592" MODIFIED="1487275489244" TEXT="Nexus braucht CoreService braucht Nexus...">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denn:
    </p>
    <ul>
      <li>
        Nexus verwendet CoreService als &quot;upstream&quot;, um <i>alle sonstigen Nachriten</i>&#160;dorthin zuzustellen
      </li>
      <li>
        CoreService hat Nexus als Upstream, um mit dem restlichen System kommunizieren zu k&#246;nnen
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
<node CREATED="1481332999091" ID="ID_1793743943" MODIFIED="1481338438364" TEXT="Problem ist: ich wollte Nexus nicht speziell konstruieren"/>
<node CREATED="1481338218216" FOLDED="true" ID="ID_128336616" MODIFIED="1488423342539" TEXT="L&#xf6;sung">
<icon BUILTIN="button_ok"/>
<node CREATED="1481338222672" ID="ID_318056010" MODIFIED="1487275489252" TEXT="Bus-Term greift tats&#xe4;chlich nicht auf Uplink zu">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint ist: im ctor
    </p>
    <p>
      Es speichert nur die Referenz
    </p>
    <p>
      
    </p>
    <p>
      Ganz anders Model::Tangible: dieses registriert sich bei der Konstruktion
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481338237550" ID="ID_781727426" MODIFIED="1481338438364" TEXT="kann also eine Referenz auf lokalen Speicher reinreichen"/>
<node CREATED="1481338295614" ID="ID_1811061645" MODIFIED="1487275489259" TEXT="Folglich mu&#xdf; Nexus lokal in CoreServices angesiedelt werden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      oder anders herum,
    </p>
    <p>
      aber so herum macht es mehr Sinn
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="forward"/>
</node>
</node>
</node>
<node CREATED="1481335696027" FOLDED="true" ID="ID_785830602" MODIFIED="1488423342657" TEXT="Lebenszyklus generell unsauber">
<node CREATED="1481335709297" ID="ID_1340554881" MODIFIED="1481338438364" TEXT="Ticket #1048"/>
<node CREATED="1481335723887" ID="ID_689818706" MODIFIED="1481338438364" TEXT="GtkLumiera ist ein Singleton">
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1481335735901" ID="ID_185489405" MODIFIED="1481338438364" TEXT="das lebt so viel zu lange">
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node CREATED="1481338348143" FOLDED="true" ID="ID_713195335" MODIFIED="1488423342657" TEXT="Entscheidung">
<icon BUILTIN="yes"/>
<node CREATED="1481338354702" ID="ID_1755264056" MODIFIED="1481338438364" TEXT="CoreServices ist das Lebenszyklus-PImpl"/>
<node CREATED="1481338363965" ID="ID_1656280824" MODIFIED="1481338438364" TEXT="wenn geschlossen, mu&#xdf; das ganze &#xfc;brige GUI schon tot sein"/>
<node CREATED="1481338393113" ID="ID_1854005469" MODIFIED="1481338438364" TEXT="Warn-Log hierf&#xfc;r eingebaut">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481338408983" ID="ID_516483369" MODIFIED="1481338438364" TEXT="k&#xf6;nnte sp&#xe4;ter sogar eine Exception sein">
<icon BUILTIN="help"/>
</node>
<node CREATED="1484871442777" ID="ID_492545805" MODIFIED="1487275489268">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      CoreService hat <i>keine volle</i>&#160;Bus-Connection
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hab ich mir jetzt explizit so &#252;berlegt und es ist sinnvoll.
    </p>
    <p>
      Nur ein Tangible kann eine volle Bus-Connection haben, und das hei&#223;t,
    </p>
    <p>
      es kann downlink-Nachrichten bekommen. Dagegen hat CoreService lediglich ein &quot;freistehendes&quot;
    </p>
    <p>
      BusTerm, das damit Nachrichten an den Nexus schicken kann.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1481320699056" ID="ID_728332859" MODIFIED="1481338454729" TEXT="wo kommen die CoreServices hin">
<icon BUILTIN="button_ok"/>
<node CREATED="1481338456152" ID="ID_266095129" MODIFIED="1481338464995" TEXT="sie leben im Objekt Ui-Bus"/>
<node CREATED="1481338465823" ID="ID_78520725" MODIFIED="1481338471698" TEXT="und zwar als PImpl"/>
<node CREATED="1481338478069" ID="ID_406101846" MODIFIED="1481338495839" TEXT="und UI-Bus lebt im lokalen scope von GtkLumiera::main()"/>
</node>
<node CREATED="1481320759472" ID="ID_687951877" MODIFIED="1481416165597" TEXT="Interfaces &#xf6;ffnen">
<icon BUILTIN="pencil"/>
<node CREATED="1481320765135" FOLDED="true" ID="ID_379585622" MODIFIED="1488423342658">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      UI: <b>GuiNotification</b>
    </p>
  </body>
</html></richcontent>
<node COLOR="#af2b37" CREATED="1484792679322" ID="ID_426218722" MODIFIED="1484792696857" TEXT="#1047"/>
<node CREATED="1481502320065" FOLDED="true" ID="ID_1298358905" MODIFIED="1488423342540" TEXT="macht bisher der GuiRunner">
<node CREATED="1481502327528" ID="ID_75293128" MODIFIED="1487275489277" TEXT="das ist ohnehin schlecht">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...weil es dadurch passieren k&#246;nnte,
    </p>
    <p>
      da&#223; die Konstruktion des GuiRunners schon scheitert, bevor der Rumpf des ctors aufgerufen wird.
    </p>
    <p>
      In einem solchen Fall wird leider auch der Rumpf des dtors nicht aufgerufen, wodurch das
    </p>
    <p>
      Term-Signal nicht ausgesendet w&#252;rde.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1481502335583" ID="ID_1245191241" MODIFIED="1481502372882" TEXT="Felder im GuiRunner sollten noexcept sein">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1484793048273" ID="ID_66499328" MODIFIED="1484793095620" TEXT="verschoben">
<arrowlink COLOR="#4072ad" DESTINATION="ID_1019455888" ENDARROW="Default" ENDINCLINATION="-334;0;" ID="Arrow_ID_693735378" STARTARROW="None" STARTINCLINATION="-45;-19;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1481502305995" FOLDED="true" ID="ID_1943234904" MODIFIED="1488423342540" TEXT="in CoreService verschieben">
<node CREATED="1484793022924" ID="ID_1019455888" MODIFIED="1484793099044" TEXT="GuiNotificattion ist jetzt Feld in CoreService">
<linktarget COLOR="#4072ad" DESTINATION="ID_1019455888" ENDARROW="Default" ENDINCLINATION="-334;0;" ID="Arrow_ID_693735378" SOURCE="ID_66499328" STARTARROW="None" STARTINCLINATION="-45;-19;"/>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1484793113472" FOLDED="true" ID="ID_221612387" MODIFIED="1488423308106" TEXT="Lebenszyklus nochmal pr&#xfc;fen">
<node CREATED="1484797307376" FOLDED="true" ID="ID_842106541" MODIFIED="1488423308091" TEXT="Abfolge">
<icon BUILTIN="info"/>
<node CREATED="1484797311008" ID="ID_424744338" MODIFIED="1484797318947" TEXT="GuiFacade::start"/>
<node CREATED="1484797337516" ID="ID_750979034" MODIFIED="1484797361213" TEXT="erzeugt unique_ptr&lt;GuiRunner&gt;"/>
<node CREATED="1484797370352" FOLDED="true" ID="ID_622906815" MODIFIED="1488423308071" TEXT="dieser hat GuiHandle als member">
<node CREATED="1484797379239" ID="ID_1948690926" MODIFIED="1484797386210" TEXT="l&#xe4;d Plugin"/>
<node CREATED="1484797399756" ID="ID_1084903350" MODIFIED="1484797408855" TEXT="ruft im ctor launchUi()"/>
</node>
<node CREATED="1484797429160" FOLDED="true" ID="ID_959278184" MODIFIED="1488423308072" TEXT="guistart.cpp">
<node CREATED="1484797432999" ID="ID_1571358017" MODIFIED="1484797439067" TEXT="implementiert GUI-Plugin"/>
<node CREATED="1484797442686" ID="ID_83446606" MODIFIED="1484797481709" TEXT="launchUI -&gt; erzeugt GUI-Thread"/>
</node>
<node CREATED="1484797483865" FOLDED="true" ID="ID_159090609" MODIFIED="1488423308072" TEXT="GuiThread (==runGUI())">
<node CREATED="1484797502462" ID="ID_21819368" MODIFIED="1484797511801" TEXT="erzeugt GuiLifecycle"/>
<node CREATED="1484797512341" ID="ID_57994807" MODIFIED="1484797515712" TEXT="ruft darauf run()"/>
<node CREATED="1484797517052" ID="ID_840628023" MODIFIED="1484797527271" TEXT="l&#xf6;st das sigTerm aus"/>
</node>
<node CREATED="1484797531482" FOLDED="true" ID="ID_1734603717" MODIFIED="1488423308072" TEXT="GuiLifecycle::run() -&gt; GtkLumiera::main">
<node CREATED="1484797543593" ID="ID_143907787" MODIFIED="1484797547740" TEXT="erzeugt UiBus"/>
<node CREATED="1484797548208" ID="ID_984623574" MODIFIED="1484797555083" TEXT="erzeugt WindowManager"/>
<node CREATED="1484797557383" ID="ID_637078214" MODIFIED="1484797568569" TEXT="Gtk-Kit::run()"/>
</node>
<node CREATED="1484797573701" FOLDED="true" ID="ID_162958920" MODIFIED="1488423308072" TEXT="UiBus">
<node CREATED="1484797576972" FOLDED="true" ID="ID_1159570219" MODIFIED="1488423308050" TEXT="erzeugt CoreService">
<node CREATED="1484871312594" ID="ID_1351400025" MODIFIED="1484871320837" TEXT="erbt zwar von BusTerm"/>
<node CREATED="1484871321473" ID="ID_112216780" MODIFIED="1487275489295" TEXT="hat aber keine connection">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint ist: keine volle bidirektionale Connection,
    </p>
    <p>
      denn CoreService ist kein Tangible. Das macht Sinn so, habe dar&#252;ber nachgedacht.
    </p>
    <p>
      
    </p>
    <p>
      Anmerkung: ein &quot;frestehendes&quot; BusTerm ist valide und zugelassen, es hat halt nur eine uplink-Connection.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484871556194" ID="ID_1978811406" MODIFIED="1487275489301" TEXT="macht Sinn so">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nur ein Tangible kann downlink-Nachrichten sinnvoll empfangen;
    </p>
    <p>
      es mu&#223; dazu auch jede Menge Methoden implementieren.
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1484797584483" FOLDED="true" ID="ID_290592673" MODIFIED="1488423308051" TEXT="dieser hat Member ">
<node CREATED="1484797623574" ID="ID_277189073" MODIFIED="1484797625783" TEXT="Nexus"/>
<node CREATED="1484797620495" ID="ID_1793464076" MODIFIED="1484797620495" TEXT="GuiNotificationService"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1484797253856" ID="ID_226245696" MODIFIED="1484871281624" TEXT="GUI start sauber">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1484871163902" ID="ID_274722396" MODIFIED="1484871283455" TEXT="closeGuiModule (PImpl) hat Lock">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1484871188635" ID="ID_1922412139" MODIFIED="1484871285111" TEXT="Storage/Lebenzyklus des Term-Funktors (jetzt) OK">
<arrowlink COLOR="#a9b4c1" DESTINATION="ID_375871642" ENDARROW="Default" ENDINCLINATION="-164;-16;" ID="Arrow_ID_896120556" STARTARROW="None" STARTINCLINATION="345;-16;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1481502251450" FOLDED="true" HGAP="96" ID="ID_1125529151" MODIFIED="1488423342540" TEXT="m&#xf6;glicher Race" VSHIFT="16">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1484797174890" ID="ID_986391148" MODIFIED="1484797217827" TEXT="Exception killt GUI bevor GuiNotification offen ist">
<icon BUILTIN="info"/>
</node>
<node CREATED="1484797210597" FOLDED="true" ID="ID_1555304752" MODIFIED="1488423308106" TEXT="ist das &#xfc;berhaupt ein Problem">
<icon BUILTIN="help"/>
<node CREATED="1484797230051" ID="ID_590420902" MODIFIED="1484797240465" TEXT="sigTerm stammt vom UI-Thread">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1481502257825" HGAP="24" ID="ID_232782099" MODIFIED="1487275489314" TEXT="Gefahr gering" VSHIFT="11">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...weil unser Thread-Framework
    </p>
    <p>
      tats&#228;chlich <i>erzwingt,</i>&#160;da&#223; der neue Thrad zu laufen beginnt, bevor die
    </p>
    <p>
      startende Funktion zur&#252;ckkehrt.
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1484797646771" FOLDED="true" HGAP="44" ID="ID_1605520996" MODIFIED="1488423342540" TEXT="Ausgestaltung" VSHIFT="10">
<icon BUILTIN="pencil"/>
<node CREATED="1484797663121" FOLDED="true" ID="ID_134980216" MODIFIED="1488423308106" TEXT="braucht Bus-Connection">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1484797863862" FOLDED="true" ID="ID_464230340" MODIFIED="1488423308091" TEXT="zum Controller machen">
<icon BUILTIN="help"/>
<node CREATED="1484797872109" ID="ID_1877007933" MODIFIED="1484797877960" TEXT="Controller ist Tangible"/>
<node CREATED="1484797878364" ID="ID_93289471" MODIFIED="1484797887414" TEXT="hat damit automatisch BusTerm"/>
<node CREATED="1484799604429" FOLDED="true" ID="ID_1141687282" MODIFIED="1488423308072" TEXT="Design-Problem: sinnvolles Diff">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1484799625306" ID="ID_443020211" MODIFIED="1484799632496" TEXT="aus den Fingern saugen">
<icon BUILTIN="help"/>
</node>
<node CREATED="1484799634233" ID="ID_1600517820" MODIFIED="1484799642178" TEXT="nur implementation-reuse">
<icon BUILTIN="help"/>
</node>
<node CREATED="1484799675388" HGAP="29" ID="ID_228932182" MODIFIED="1484799693431" TEXT="Idee: Attribute?" VSHIFT="18">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1484876010871" ID="ID_175784507" MODIFIED="1484876038858" TEXT="Design-Problem: restliches Tangible-Protokoll">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wirkt alles mehr oder weniger beliebig...
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484875935505" ID="ID_1341415727" MODIFIED="1484875940799" TEXT="nein">
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node COLOR="#338800" CREATED="1484875946312" FOLDED="true" ID="ID_271363232" MODIFIED="1488423308091" TEXT="ist ein BusTerm">
<icon BUILTIN="button_ok"/>
<node CREATED="1484875959342" ID="ID_1202046492" MODIFIED="1484875967689" TEXT="nur unidirektional zum Bus verbunden"/>
<node CREATED="1484875968557" ID="ID_1406569407" MODIFIED="1484875989502" TEXT="ist hier genau angemessen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484875972588" ID="ID_1887863793" MODIFIED="1484875984841" TEXT="damit kann der Service mit dem UI reden">
<icon BUILTIN="idea"/>
</node>
</node>
<node COLOR="#338800" CREATED="1484799525344" FOLDED="true" ID="ID_640788614" MODIFIED="1488423308091" TEXT="mu&#xdf; sauber vom Nexus disconnecten">
<icon BUILTIN="button_ok"/>
<node COLOR="#ff0000" CREATED="1484799538686" ID="ID_1136883698" MODIFIED="1484873804862" TEXT="tut es nicht">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1484799551220" ID="ID_496503271" MODIFIED="1484799585260" TEXT="Fehlermeldung + Segfault">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      0000000937: ERR: core-service.hpp:111: worker_3: ~CoreService: Some UI components are still connected to the backbone.
    </p>
    <p>
      Speicherzugriffsfehler
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484800480472" ID="ID_1441013524" MODIFIED="1484800529641" TEXT="Sanity-Check in CoreService anpassen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...mu&#223; diejenigen Bus-Verbindungen abziehen, die von Members dieser Klasse stammen
    </p>
    <ul>
      <li>
        CoreService selber
      </li>
      <li>
        der NotificationService
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484802745625" FOLDED="true" ID="ID_375871642" MODIFIED="1488423308072" TEXT="Heisenbug">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_375871642" ENDARROW="Default" ENDINCLINATION="-164;-16;" ID="Arrow_ID_896120556" SOURCE="ID_1922412139" STARTARROW="None" STARTINCLINATION="345;-16;"/>
<icon BUILTIN="smily_bad"/>
<node CREATED="1484802752728" ID="ID_634426848" MODIFIED="1484802766354" TEXT="passiert beim Aufruf des TerminationHandle"/>
<node CREATED="1484802766966" ID="ID_370736554" MODIFIED="1484802805848">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn ich per Value capture, dann gibts schon
    </p>
    <p>
      beim Start des GUI einen SEGFAULT
    </p>
    <p>
      
    </p>
    <p>
      Und noch schlimmer: im Debugger gibts keinen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node COLOR="#c80e33" CREATED="1484862058589" FOLDED="true" ID="ID_1640490030" MODIFIED="1488423308051" TEXT="Locking">
<icon BUILTIN="help"/>
<node COLOR="#338800" CREATED="1484865203945" ID="ID_1990023724" MODIFIED="1484866438758" TEXT="Locking in der GuiFacade ist korrekt">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484865231878" ID="ID_1054093321" MODIFIED="1484865240952" TEXT="im Gui-Thread kein Locking notwendig"/>
<node CREATED="1484865243724" ID="ID_617730067" MODIFIED="1484865273473" TEXT="HA! das Lambda vom Threadstart">
<icon BUILTIN="idea"/>
</node>
</node>
<node COLOR="#338800" CREATED="1484865278999" FOLDED="true" ID="ID_1656236909" MODIFIED="1488423308051" TEXT="aufgekl&#xe4;rt">
<icon BUILTIN="button_ok"/>
<node CREATED="1484865298093" ID="ID_1797078596" MODIFIED="1484865314802">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die <i>Closure</i>&#160;eines Lambdas h&#228;ngt am Kontext
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484865316147" ID="ID_687373412" MODIFIED="1484865348826" TEXT="wenn ich den Thread mit Lambda starte..."/>
<node CREATED="1484865349366" ID="ID_652410738" MODIFIED="1484865350930" TEXT="betrifft das den term-Functor"/>
<node CREATED="1484865513568" ID="ID_696204655" MODIFIED="1484865622731" TEXT="selbst wenn der Funktor selber per Value &#xfc;bergeben wird">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      konkret:
    </p>
    <p>
      der Kontext ist hier n&#228;mlich ein anderes Lambda, das dem Aufruf des GUI-Plugins mitgegeben wurde.
    </p>
    <p>
      Dagegen die alte L&#246;sung erzeugte an dieser Stelle einen Bind-Ausdruck, und das war offenbar genug,
    </p>
    <p>
      um nicht mehr von dem direkten Kontext abh&#228;ngig zu sein, in dem der Thread gestartet wurde.
    </p>
    <p>
      
    </p>
    <p>
      Denn dieser Kontext (auf dem Stack) ist nat&#252;rlich lange schon weg, wenn der Thread
    </p>
    <p>
      terminiert und dann tats&#228;chlich den Fuktor aufrufen m&#246;chte
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1484800741349" FOLDED="true" ID="ID_1156360682" MODIFIED="1488423308072">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Debug: nur <b>ein</b>&#160;Element connected
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1484800781408" ID="ID_183164357" MODIFIED="1484875241862" TEXT="n&#xe4;mlich der NotificationService">
<icon BUILTIN="help"/>
</node>
<node COLOR="#338800" CREATED="1484875214137" ID="ID_977859942" MODIFIED="1484875237383" TEXT="nein: das ist genau OK">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484875222128" ID="ID_1133941591" MODIFIED="1484875235519" TEXT="CoreService ist nicht bidirektional angebunden">
<icon BUILTIN="info"/>
</node>
</node>
</node>
<node CREATED="1484876114521" FOLDED="true" ID="ID_1082250595" MODIFIED="1488423308091" TEXT="Protokoll zur Kommunikation">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1484876122960" ID="ID_31825421" MODIFIED="1484882554013" TEXT="UI-Bus nur im GUI-Thread">
<linktarget COLOR="#b7748e" DESTINATION="ID_31825421" ENDARROW="Default" ENDINCLINATION="560;150;" ID="Arrow_ID_1434676575" SOURCE="ID_935689465" STARTARROW="Default" STARTINCLINATION="587;44;"/>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1484876143949" ID="ID_558559630" MODIFIED="1484876153688" TEXT="mu&#xdf; Nachrichten explizit &#xfc;bergeben"/>
<node CREATED="1484876154492" ID="ID_669377427" MODIFIED="1484876157575" TEXT="an Locking denken"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1484797900185" FOLDED="true" ID="ID_1895372084" MODIFIED="1488423308091" TEXT="Protokoll zum Schlie&#xdf;en">
<icon BUILTIN="flag-yellow"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1484797911392" ID="ID_286857196" MODIFIED="1485126732565" TEXT="mu&#xdf; ich definieren">
<linktarget COLOR="#872666" DESTINATION="ID_286857196" ENDARROW="Default" ENDINCLINATION="272;63;" ID="Arrow_ID_1606936924" SOURCE="ID_405470138" STARTARROW="Default" STARTINCLINATION="1461;0;"/>
<icon BUILTIN="flag-pink"/>
</node>
<node CREATED="1484797920846" ID="ID_472054508" MODIFIED="1484797941875" TEXT="sollte Nachricht auf dem Bus sein">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1484797928661" ID="ID_256413655" MODIFIED="1484797937983" TEXT="sollte in das bestehende Protokoll passen">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1484797684438" FOLDED="true" ID="ID_932852399" MODIFIED="1488423308107" TEXT="GUI-Shutdown implementieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bisher k&#246;nnen wir das GUI nur <i>aktiv intern</i>&#160;schlie&#223;en,
    </p>
    <p>
      indem wir ein GTK-Signal erzeugen, das das Hauptfenster schlie&#223;t
    </p>
  </body>
</html></richcontent>
<node CREATED="1484797749237" ID="ID_1654016697" MODIFIED="1484797763392" TEXT="wie kann man GTK-UI explizit schlie&#xdf;en">
<icon BUILTIN="help"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1484797766035" ID="ID_1740926324" MODIFIED="1484797792293" TEXT="wie signalisieren wir das">
<icon BUILTIN="help"/>
</node>
<node CREATED="1485116867963" FOLDED="true" ID="ID_1146024699" MODIFIED="1488423308091" TEXT="bisher kann man nur das Fenster schlie&#xdf;en">
<node CREATED="1485116884925" ID="ID_296160451" MODIFIED="1485116898396" TEXT="wie funktioniert das?">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="help"/>
</node>
<node CREATED="1485116901507" FOLDED="true" ID="ID_1272809751" MODIFIED="1488423308072" TEXT="WindowManager">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485126609878" ID="ID_649614174" MODIFIED="1485126614405" TEXT="#1064 investigate WindowManager lifecycle">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1485116921072" ID="ID_1191743111" MODIFIED="1485116925722" TEXT="close Action"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488419591265" FOLDED="true" ID="ID_471609909" MODIFIED="1488423308092">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      zieht komplett-Umbau
    </p>
    <p>
      des Gui-top-Level nach sich
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#851358" DESTINATION="ID_548720270" ENDARROW="Default" ENDINCLINATION="-663;-614;" ID="Arrow_ID_325703166" STARTARROW="None" STARTINCLINATION="1209;488;"/>
<icon BUILTIN="messagebox_warning"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485118623744" FOLDED="true" ID="ID_392996871" MODIFIED="1488423308073" TEXT="#1032 use gtk::Application instead of gtk::Main">
<arrowlink COLOR="#ae1856" DESTINATION="ID_206480879" ENDARROW="Default" ENDINCLINATION="715;0;" ID="Arrow_ID_926482654" STARTARROW="Default" STARTINCLINATION="134;383;"/>
<icon BUILTIN="pencil"/>
<node CREATED="1485118668802" ID="ID_575152579" MODIFIED="1485118676149" TEXT="WindowManager verwendet gtk::Main"/>
<node CREATED="1485118677425" ID="ID_1952311126" MODIFIED="1485118685915" TEXT="genau, um die Applikation runterzufahren"/>
<node CREATED="1485124031945" FOLDED="true" ID="ID_329247026" MODIFIED="1488423308051" TEXT="Gtk::Application">
<node CREATED="1485124038768" ID="ID_764043255" MODIFIED="1485124045970" TEXT="ist kein Singleton"/>
<node CREATED="1485124046775" ID="ID_505212945" MODIFIED="1485124051706" TEXT="hat keinen public ctor"/>
<node CREATED="1485124052149" ID="ID_799710500" MODIFIED="1485124060680" TEXT="mit create erstellt -&gt; smart ptr"/>
<node CREATED="1485124063732" FOLDED="true" ID="ID_1584553859" MODIFIED="1488423306817" TEXT="erbt von Gio::Application">
<node CREATED="1485124070131" ID="ID_1720443251" MODIFIED="1485124077430" TEXT="wrappt g_application"/>
</node>
</node>
<node CREATED="1485124084465" FOLDED="true" ID="ID_660066920" MODIFIED="1488423308052" TEXT="how-to">
<icon BUILTIN="help"/>
<node CREATED="1485124094936" FOLDED="true" ID="ID_1446212765" MODIFIED="1488423306817" TEXT="offiziell">
<node CREATED="1485124098920" ID="ID_1783958965" MODIFIED="1485124109274" TEXT="auto app = Application::create"/>
<node CREATED="1485124111806" ID="ID_231977368" MODIFIED="1485124114842" TEXT="app-&gt;run"/>
</node>
<node CREATED="1485124120220" FOLDED="true" ID="ID_1639025787" MODIFIED="1488423306817" TEXT="von Application erben">
<icon BUILTIN="help"/>
<node CREATED="1485124129436" ID="ID_1540860407" MODIFIED="1485124141373" TEXT="laut Quellcode spricht nichts dagegen"/>
<node CREATED="1485124142178" ID="ID_1048261084" MODIFIED="1487275489352" TEXT="man sollte aber keine Methoden &#xfc;berschreiben">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...nur eine <i>heuristische </i>Vermutung von mir
    </p>
    <p>
      st&#252;tzt sich auf folgenden Quellcode
    </p>
    <p>
      
    </p>
    <p>
      Application::Application(const Glib::ustring&amp; application_id, Gio::ApplicationFlags flags)
    </p>
    <p>
      :
    </p>
    <p>
      &#160;&#160;// Mark this class as non-derived to allow C++ vfuncs to be skipped.
    </p>
    <p>
      &#160;&#160;//Note that GApplication complains about &quot;&quot; but allows NULL (0), so we avoid passing &quot;&quot;.
    </p>
    <p>
      &#160;&#160;Glib::ObjectBase(0),
    </p>
    <p>
      &#160;&#160;Gio::Application(Glib::ConstructParams(custom_class_init(), &quot;application_id&quot;, (application_id.empty() ? 0 : application_id.c_str()), &quot;flags&quot;, GApplicationFlags(flags), static_cast&lt;char*&gt;(0))),
    </p>
    <p>
      &#160;&#160;m_argc(0),
    </p>
    <p>
      &#160;&#160;m_argv(0)
    </p>
    <p>
      {
    </p>
    <p>
      &#160;&#160;gtk_init(0, 0);
    </p>
    <p>
      }
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1485124284856" FOLDED="true" ID="ID_473768869" MODIFIED="1488423308052" TEXT="Windows selber managen">
<icon BUILTIN="help"/>
<node CREATED="1485124296582" ID="ID_628648705" MODIFIED="1485124304593" TEXT="gtk::Application verwaltet Fensterliste"/>
<node CREATED="1485124305157" ID="ID_1632068669" MODIFIED="1485124313311" TEXT="schlie&#xdf;t sich automatisch, wenn das letzte Fenster zu ist"/>
<node CREATED="1485124315795" ID="ID_178656069" MODIFIED="1485124323478" TEXT="unser WindowManager macht das Gleiche"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485130302095" FOLDED="true" HGAP="28" ID="ID_506541774" MODIFIED="1488423308073" TEXT="neuer UiManager" VSHIFT="14">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1485452508180" FOLDED="true" ID="ID_549617075" MODIFIED="1488423308052" TEXT="wird daf&#xfc;r zust&#xe4;ndig">
<icon BUILTIN="button_cancel"/>
<node CREATED="1485130321924" ID="ID_46787934" MODIFIED="1485130327728" TEXT="Funktion umziehen"/>
<node CREATED="1485130328324" ID="ID_874057448" MODIFIED="1485130334983" TEXT="umschreiben auf gtk::Application"/>
<node CREATED="1485130336235" ID="ID_125642886" MODIFIED="1485130353981" TEXT="Application-Objekt durchgeben"/>
<node CREATED="1485130355377" ID="ID_175334188" MODIFIED="1485130395487" TEXT="Zugriff f&#xfc;r NotificationFacade erm&#xf6;glichen"/>
</node>
<node CREATED="1485452536138" FOLDED="true" ID="ID_599315721" MODIFIED="1488423308052" TEXT="vermittelt das">
<icon BUILTIN="pencil"/>
<node CREATED="1485452541583" ID="ID_790081376" MODIFIED="1485452548314" TEXT="Funktor speichern"/>
<node CREATED="1485452548693" ID="ID_375213971" MODIFIED="1485452552248" TEXT="Sicherheit des Funktors"/>
<node CREATED="1485130355377" ID="ID_483167177" MODIFIED="1485130395487" TEXT="Zugriff f&#xfc;r NotificationFacade erm&#xf6;glichen"/>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1484797808349" FOLDED="true" ID="ID_389264738" MODIFIED="1488423308107" TEXT="weitere Funktionen">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1484797813373" ID="ID_536855363" MODIFIED="1484797818072" TEXT="generische Nachricht"/>
<node CREATED="1484797818780" FOLDED="true" ID="ID_354397333" MODIFIED="1488423308092" TEXT="state mark">
<node CREATED="1484877574046" ID="ID_251939815" MODIFIED="1484877577130" TEXT="Error"/>
<node CREATED="1484877577550" ID="ID_258243919" MODIFIED="1484877578530" TEXT="Note"/>
</node>
<node CREATED="1484797822403" FOLDED="true" ID="ID_364545191" MODIFIED="1488423308092" TEXT="Diff">
<node CREATED="1484799660822" ID="ID_1288003894" MODIFIED="1484799667505" TEXT="TreeMutator binden"/>
<node CREATED="1484877585605" ID="ID_101718446" MODIFIED="1484877768706" TEXT="verwende MutationMessage">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1484877590492" ID="ID_1552177670" MODIFIED="1487275489371" TEXT="k&#xf6;nnte sich sp&#xe4;ter noch &#xc4;ndern">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....aber im Moment sieht das ad&#228;quat aus,
    </p>
    <p>
      da ja der Konstruktor von MutationMessage generisch (Template) ist,
    </p>
    <p>
      und man damit jedes diff-iterable einbetten kann.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484877658891" FOLDED="true" ID="ID_1855662071" MODIFIED="1488423308073" TEXT="Vorsicht: Storage / GUI-Thread">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1484877696486" ID="ID_476867583" MODIFIED="1484877708576" TEXT="wir m&#xfc;ssen jede Aktion in den Event-Thread &#xfc;bergeben"/>
<node CREATED="1484877711388" ID="ID_407918505" MODIFIED="1484877730584">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, die Diff-<b>Implementierung</b>&#160;mu&#223; l&#228;nger leben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484877731337" ID="ID_1859059266" MODIFIED="1484877761556">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      faktisch erfolgt somit ein <b>Callback</b>&#160;aus einem anderen Thread
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1484877743992" ID="ID_680582075" MODIFIED="1484877751765">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      um das Diff zu <i>pullen</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1484877839299" FOLDED="true" HGAP="26" ID="ID_957622878" MODIFIED="1488423342540" TEXT="Asynchrones Protokoll" VSHIFT="1">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1484877856640" ID="ID_1256835676" MODIFIED="1484877867427" TEXT="bedenke: alle Aktionen m&#xfc;ssen in den Event-Thread"/>
<node CREATED="1484877868015" ID="ID_1612427727" MODIFIED="1484877878730" TEXT="&#xfc;ber die Rolle des GUI-Modells nachdenken!"/>
</node>
</node>
<node CREATED="1481320772830" FOLDED="true" HGAP="21" ID="ID_1607125695" MODIFIED="1490986155474" VSHIFT="11">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Proc: <b>SessionCommand</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node COLOR="#338800" CREATED="1481320843596" ID="ID_7225903" MODIFIED="1481509899511" TEXT="definieren">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481767553542" FOLDED="true" ID="ID_1936508244" MODIFIED="1488423342540" TEXT="Eigenschaften">
<icon BUILTIN="info"/>
<node CREATED="1481768183970" ID="ID_1887870402" MODIFIED="1481768197029" TEXT="ist offizielle Session-Schnittstelle"/>
<node CREATED="1481768211951" ID="ID_287828489" MODIFIED="1481768529260">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      setzt <i>aktivierten</i>&#160;Dispatcher <font color="#6e080d">zwingend</font>&#160;voraus
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es gen&#252;gt definitiv nicht, nur die Dispatcher-Komponente(Schnittstelle) erreichen zu k&#246;nnen.
    </p>
    <p>
      Jede Operation, die &#252;ber dieses externe Interface bereitsteht, ben&#246;tigt zur Implementierung
    </p>
    <p>
      eine aktiv laufende Dispatcher-Queue.
    </p>
    <p>
      
    </p>
    <p>
      Daher macht es Sinn, den Interface-Lebenszyklus ganz starr an den Disspatcher zu binden
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481768341149" ID="ID_1200042703" MODIFIED="1481768435704" TEXT="implizite Bindung an eine Session-Instanz">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und zwar wirklich sehr implizit,
    </p>
    <p>
      n&#228;mlich &#252;ber die Identit&#228;t (IDs) der Command-Parameter.
    </p>
    <p>
      Das hei&#223;t, ein eingehendes Command pa&#223;t nur zu einer bestimmten Session-Instanz,
    </p>
    <p>
      was zwar jederzeit (via statisches/internes Session-API) verifizierbar ist, jedoch nicht offensichtlich
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481768566167" ID="ID_702026185" MODIFIED="1481768669948" TEXT="kann jederzeit asynchron geschlossen werden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das folgt einfach aus den logischen Eigenschaften der beteiligten Komponenten,
    </p>
    <p>
      welche eben autonom sind.
    </p>
    <p>
      
    </p>
    <p>
      Das hei&#223;t im Klartext, alle Clients m&#252;ssen darauf vorbereitet sein, da&#223; diese Schnittstelle
    </p>
    <p>
      <i>jederzeit</i>&#160;wegbrechen kann, was dann hei&#223;t, da&#223; irgend ein Aufruf eine Exception wirft
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1481320847724" FOLDED="true" HGAP="42" ID="ID_545146501" MODIFIED="1488423342540" TEXT="wo ansiedeln" VSHIFT="-6">
<icon BUILTIN="help"/>
<node CREATED="1481509901926" ID="ID_1612540786" MODIFIED="1481509907305" TEXT="in proc::control"/>
<node CREATED="1481767522123" FOLDED="true" ID="ID_789146708" MODIFIED="1488423308107">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wer besitzt die
    </p>
    <p>
      Implementierung
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1481767563781" FOLDED="true" ID="ID_1894117297" MODIFIED="1488423308092" TEXT="Subsystem">
<icon BUILTIN="button_cancel"/>
<node CREATED="1481767649426" FOLDED="true" ID="ID_1806364059" MODIFIED="1488423308073" TEXT="pro">
<node CREATED="1481767653785" ID="ID_65381960" MODIFIED="1481767658028" TEXT="offensichtlich"/>
<node CREATED="1481767700299" ID="ID_1163247183" MODIFIED="1481767705134" TEXT="logisch"/>
</node>
<node CREATED="1481767665231" FOLDED="true" ID="ID_141423591" MODIFIED="1488423308073" TEXT="con">
<node CREATED="1481767681797" ID="ID_1133339398" MODIFIED="1481768995389" TEXT="fragiler Lebenszyklus">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      meint: zwei gekoppelte Statusvariable
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481767706906" ID="ID_815807707" MODIFIED="1481767710581" TEXT="bekomme dort Zustand"/>
<node CREATED="1481767720576" ID="ID_714877064" MODIFIED="1481767729611" TEXT="mu&#xdf; dort Locking verwenden"/>
<node CREATED="1481767711033" ID="ID_232055384" MODIFIED="1481767716628" TEXT="bekomme dort Fehlerbehandlung"/>
</node>
</node>
<node CREATED="1481767579130" FOLDED="true" ID="ID_191292506" MODIFIED="1488423308092" TEXT="Dispatcher">
<icon BUILTIN="help"/>
<node CREATED="1481767748180" FOLDED="true" ID="ID_549036367" MODIFIED="1488423308073" TEXT="pro">
<node CREATED="1481767761027" ID="ID_949448369" MODIFIED="1481767771781" TEXT="ist die offizielle Schnittstelle"/>
<node CREATED="1481767864069" ID="ID_47595319" MODIFIED="1481767874567" TEXT="kann Service-Impl direkt verdrahten"/>
</node>
<node CREATED="1481767758347" FOLDED="true" ID="ID_1773256683" MODIFIED="1488423308073" TEXT="con">
<node CREATED="1481767797062" ID="ID_1542908203" MODIFIED="1481768960633">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mu&#223; alle Operationen durchschleifen
    </p>
    <p>
      oder mu&#223; PImpl als Interface exponieren
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481767831105" ID="ID_1351930491" MODIFIED="1481768989667" TEXT="fragiler Lebenszyklus">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      meint: zwei gekoppelte Statusvariable
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1481767581619" FOLDED="true" ID="ID_1364443356" MODIFIED="1488423308092" TEXT="DispatcherLoop">
<linktarget COLOR="#4aff51" DESTINATION="ID_1364443356" ENDARROW="Default" ENDINCLINATION="10;30;" ID="Arrow_ID_870320696" SOURCE="ID_1152351588" STARTARROW="Default" STARTINCLINATION="-221;-88;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1481767877379" FOLDED="true" ID="ID_1139131626" MODIFIED="1488423308073" TEXT="pro">
<node CREATED="1481767893969" ID="ID_1688865092" MODIFIED="1481767911170" TEXT="kann dort dediziertes Interface nutzen"/>
<node CREATED="1481767924909" ID="ID_438801896" MODIFIED="1481767931359" TEXT="alle Operationen passieren dort"/>
<node CREATED="1481767961616" FOLDED="true" ID="ID_1097715033" MODIFIED="1488423308052" TEXT="Lebenszyklus == RAII">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1481769041800" ID="ID_1026761428" MODIFIED="1481769059178" TEXT="ausschlaggebend">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1481769052246" ID="ID_628010771" MODIFIED="1481769055002" TEXT="gutes Design"/>
</node>
<node CREATED="1481768131609" ID="ID_600248646" MODIFIED="1481768139171" TEXT="Locking schon da"/>
</node>
<node CREATED="1481768014089" FOLDED="true" ID="ID_1260583976" MODIFIED="1488423308074" TEXT="con">
<node CREATED="1481768019848" ID="ID_1155187340" MODIFIED="1481768032040">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Shutdown <i>tricky</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481768075481" ID="ID_795971368" MODIFIED="1481768083275" TEXT="in der Implementierung versteckt"/>
</node>
</node>
</node>
</node>
<node CREATED="1481320850779" FOLDED="true" HGAP="45" ID="ID_1632600003" MODIFIED="1488423342540" TEXT="hochfahren" VSHIFT="-3">
<icon BUILTIN="button_ok"/>
<node CREATED="1481509921211" FOLDED="true" ID="ID_1515469097" MODIFIED="1488423308107">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Session-Subsystem implementieren <font color="#c60814">(#318)</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node COLOR="#338800" CREATED="1481599413419" FOLDED="true" HGAP="34" ID="ID_673133356" MODIFIED="1488423308092" TEXT="Ticket #318" VSHIFT="-7">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....das ist schon mehr ein Meta-Ticket,
    </p>
    <p>
      und es h&#228;ngt wohl zu viel darunter, um es gleich ganz abschlie&#223;en zu k&#246;nnen.
    </p>
    <p>
      Aber ich <i>akzeptiere</i>&#160;es und verwende es jetzt als Treiber
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node COLOR="#338800" CREATED="1483473104426" ID="ID_1606817357" MODIFIED="1483747983243" TEXT="#701 Session interface Support facility">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1483473212107" FOLDED="true" ID="ID_600424316" MODIFIED="1488423308074" TEXT="#699 entrance to session subsystem">
<icon BUILTIN="button_ok"/>
<node CREATED="1483748003185" ID="ID_1342405777" MODIFIED="1483748021355" TEXT="sollte das auch tats&#xe4;chlich mal in Betrieb setzen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1483748023934" ID="ID_535793074" MODIFIED="1483753100683" TEXT="Session ab jetzt in main() hochfahren...">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node COLOR="#338800" CREATED="1483473153851" FOLDED="true" ID="ID_868700871" MODIFIED="1488423308074" TEXT="#1049 draft SessionLoop component">
<icon BUILTIN="button_ok"/>
<node CREATED="1483642220941" ID="ID_517844262" MODIFIED="1483645251383" TEXT="integrate API functions for enqueuing commands">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483642220941" ID="ID_775655275" MODIFIED="1483645257684" TEXT="integrate actual command queue">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483642220941" ID="ID_704571897" MODIFIED="1483648766529" TEXT="care for waking (notification) from sleep state">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483642220940" ID="ID_1519554797" MODIFIED="1483654058917" TEXT="care for syncing the disabled state">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483642220941" ID="ID_377776032" MODIFIED="1483746849507" TEXT="deadlock safeguard">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1482464327133" FOLDED="true" HGAP="51" ID="ID_996095134" MODIFIED="1488423308092" TEXT="was es ist" VSHIFT="-2">
<icon BUILTIN="help"/>
<node CREATED="1482464377831" ID="ID_1028012453" MODIFIED="1482464390602">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>ist nicht</i>&#160;&quot;die Session
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
<node CREATED="1482464332981" ID="ID_1523357281" MODIFIED="1482464350688" TEXT="Session Subsystem == Session Dispatcher Loop Thread">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482464414721" ID="ID_490065319" MODIFIED="1482464436563" TEXT="verwaltet Command-Queue"/>
<node CREATED="1482464436919" ID="ID_64274402" MODIFIED="1482464442042" TEXT="f&#xfc;hrt Commands aus"/>
<node CREATED="1482464442438" ID="ID_1339868449" MODIFIED="1482464449049" TEXT="startet den Builder"/>
</node>
<node CREATED="1481599421021" FOLDED="true" ID="ID_461768132" MODIFIED="1488423308092" TEXT="Locking kl&#xe4;ren">
<icon BUILTIN="button_ok"/>
<node CREATED="1481684383530" FOLDED="true" ID="ID_236117" MODIFIED="1488423308074" TEXT="zwei unabh&#xe4;ngige Locks">
<icon BUILTIN="yes"/>
<node CREATED="1481684391921" ID="ID_138369898" MODIFIED="1481684442846" TEXT="Front-End">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das Lock sorgt hier f&#252;r konsistenten Zustand und Sichtbarkeit (memory barrier)
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481684394544" ID="ID_1314197501" MODIFIED="1481684422640" TEXT="Back-End">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Lock ist hier das Dispatcher-Lock
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1481599468712" FOLDED="true" ID="ID_1273965744" MODIFIED="1488423308074" TEXT="Vorsicht beim Anhalten (Deadlock-Gefahr)">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1483477519974" ID="ID_1731425552" MODIFIED="1483477535864" TEXT="Term-Signal k&#xf6;nnte blocken">
<icon BUILTIN="help"/>
</node>
<node COLOR="#338800" CREATED="1483484052785" ID="ID_667055412" MODIFIED="1483581561638" TEXT="Race beim runningLoop_.reset()">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1483490459499" ID="ID_1006586365" MODIFIED="1483581568462" TEXT="Thread will sich selbst reapen ==&gt; Deadlock">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483581573147" FOLDED="true" ID="ID_989149672" MODIFIED="1488423308053" TEXT="durch Umbau gel&#xf6;st">
<icon BUILTIN="info"/>
<node CREATED="1483490316758" ID="ID_1734989824" MODIFIED="1483581603244" TEXT="nur das Term-Signal kann DispatcherLoop deleten">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1483581605968" ID="ID_227347532" MODIFIED="1483581655738" TEXT="mu&#xdf; das &#xe4;u&#xdf;ere Lock auf ProcDispatcher verwenden"/>
<node CREATED="1483581657849" ID="ID_1078225675" MODIFIED="1483581670643" TEXT="damit sind konkurrierende Zugriffe auf die Loop geblockt"/>
<node CREATED="1483581671183" ID="ID_186079754" MODIFIED="1483581680474" TEXT="und danach ist der PImpl bereits auf NULL gesetzt"/>
<node CREATED="1483581583411" ID="ID_650761020" MODIFIED="1483581594997" TEXT="thread join ist &#xfc;berfl&#xfc;ssig"/>
<node CREATED="1483581640739" ID="ID_560356092" MODIFIED="1483581652445" TEXT="das Objekt wird in einem tail-call gel&#xf6;scht"/>
<node CREATED="1483581682701" ID="ID_747399934" MODIFIED="1483581695879" TEXT="dieser callback selber vewrwendet nur Werte vom Stack"/>
<node CREATED="1483581696443" ID="ID_373783285" MODIFIED="1483581733454" TEXT="daher kann das Objekt gefahrlos &quot;sich selber l&#xf6;schen&quot;">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1482464502542" ID="ID_284397306" MODIFIED="1482464609118" TEXT="Vorsicht beim Schlie&#xdf;en der Session">
<arrowlink COLOR="#5a97df" DESTINATION="ID_1378897740" ENDARROW="Default" ENDINCLINATION="9540;0;" ID="Arrow_ID_1446486242" STARTARROW="None" STARTINCLINATION="9540;0;"/>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1481599430804" FOLDED="true" HGAP="19" ID="ID_1105415989" MODIFIED="1488423308093" TEXT="Forwarding-Operationen implementieren" VSHIFT="10">
<icon BUILTIN="button_ok"/>
<node CREATED="1481686902156" ID="ID_1526215617" MODIFIED="1481686910633" TEXT="aktivieren /deaktivieren">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481686911938" ID="ID_17287395" MODIFIED="1483581788279" TEXT="stoppen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1481599594399" FOLDED="true" ID="ID_1017459823" MODIFIED="1488423308093" TEXT="entscheiden, wer die Session &#xf6;ffnet">
<icon BUILTIN="button_ok"/>
<node CREATED="1481686982097" ID="ID_1925999031" MODIFIED="1481687002238" TEXT="passiert on demand">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wenn jemand zugreift
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481599603757" ID="ID_812618450" MODIFIED="1481599612320" TEXT="Operation auf dem SessionManager"/>
<node CREATED="1481599612812" ID="ID_1970476027" MODIFIED="1481599618598" TEXT="der ist Singleton"/>
<node CREATED="1481599626194" ID="ID_1815350592" MODIFIED="1481599638105" TEXT="wenn Session &#xf6;ffnet, wird der Dispatcher aktiviert">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1481687005813" ID="ID_1108477992" MODIFIED="1481687081246" TEXT="Entscheidung: Sesion-lifecycle und Dispatcher sind unabh&#xe4;ngig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      grundlegende Design-Enscheidung
    </p>
    <ul>
      <li>
        wir haben Komponenten mit Dependency-Injection
      </li>
      <li>
        da beide Komponenten nur nach ihren eigenen Hinsichten funktionieren,<br />wird das System insgesamt einfacher
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
<node COLOR="#338800" CREATED="1483581821347" FOLDED="true" HGAP="13" ID="ID_1893571865" MODIFIED="1488423308093" TEXT="Review Lebenszyklus" VSHIFT="8">
<icon BUILTIN="button_ok"/>
<node CREATED="1483581829682" ID="ID_1563520696" MODIFIED="1483581904481" TEXT="alle Subsys-Operationen auf Semantik durchgepr&#xfc;ft">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483581852071" ID="ID_1139549600" MODIFIED="1483581902223" TEXT="1/2017: Locking und Concurrency erscheint sauber behandelt">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1481510231986" FOLDED="true" HGAP="23" ID="ID_45803267" MODIFIED="1488423308107" TEXT="dort SessionCommandService instantiieren" VSHIFT="4">
<icon BUILTIN="button_ok"/>
<node CREATED="1481769089234" ID="ID_1152351588" MODIFIED="1483582218575" TEXT="lebt in der DispatcherLoop">
<arrowlink COLOR="#4aff51" DESTINATION="ID_1364443356" ENDARROW="Default" ENDINCLINATION="10;30;" ID="Arrow_ID_870320696" STARTARROW="Default" STARTINCLINATION="-221;-88;"/>
<icon BUILTIN="info"/>
</node>
<node CREATED="1481769144226" ID="ID_1609251574" MODIFIED="1482532883149" TEXT="mu&#xdf; Service-API extrahieren">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481777039358" FOLDED="true" ID="ID_395609084" MODIFIED="1488423308093" TEXT="Shutdown sauber regeln">
<icon BUILTIN="button_ok"/>
<node CREATED="1481777054755" FOLDED="true" ID="ID_1551225439" MODIFIED="1488423308074">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mu&#223; SessionCommandService schlie&#223;en
    </p>
    <p>
      <i>bevor</i>&#160;die Dispatcher-loop angehalten wird
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1481778397600" ID="ID_1997192349" MODIFIED="1481778409498" TEXT="bereits wenn Anhalten signalisiert wird"/>
<node CREATED="1481778409942" ID="ID_100284739" MODIFIED="1481778419113" TEXT="zur Sicherheit nochmal im dtor"/>
</node>
<node CREATED="1481777114203" ID="ID_1156396729" MODIFIED="1483654726680" TEXT="Lock, um Deadlock zu vermeiden"/>
<node CREATED="1481777122306" ID="ID_1506554988" MODIFIED="1481777133445" TEXT="alles im dtor  -&gt; noexcept"/>
<node CREATED="1481777210447" ID="ID_1392452935" MODIFIED="1483654905825" TEXT="sicherstellen da&#xdf; shutdown nicht blockt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      1/2017 Review durchgef&#252;hrt und Logik &#252;berarbeitet.
    </p>
    <p>
      Einziger Risikofaktor ist nun, wenn beim Schlie&#223;en des SessionCommand-Intertfaces
    </p>
    <p>
      oder beim Signalisieren an den Thread eine Exception fliegt; denn dann loggen wir zwar,
    </p>
    <p>
      aber die Shutdown-R&#252;ckmeldung kommt u.U niemals an, und damit bleiben wir
    </p>
    <p>
      am Ende von main() einfach h&#228;ngen.
    </p>
    <p>
      
    </p>
    <p>
      <i>Ich halte diese F&#228;lle aber f&#252;r in der Praxis nicht relevant,</i>&#160; und verzichte daher auf eine Spezialbehandlung
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#5a97df" DESTINATION="ID_1789585729" ENDARROW="Default" ENDINCLINATION="3216;0;" ID="Arrow_ID_1930701586" STARTARROW="None" STARTINCLINATION="8475;0;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1481510244184" ID="ID_1521406724" MODIFIED="1481510260635" TEXT="kann dann in CoreService einfach &#xfc;ber die .facade() zugreifen">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1483748175610" FOLDED="true" HGAP="31" ID="ID_73597248" MODIFIED="1490986131069" VSHIFT="16">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Einfachen Aufruf
    </p>
    <p>
      implementieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node COLOR="#338800" CREATED="1483748214772" ID="ID_1642552563" MODIFIED="1484377266424" TEXT="#1046 Preliminary Definition of SessionCommand facade">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483748231330" ID="ID_1962213649" MODIFIED="1483748356929" TEXT="soll hierzu sehen, da&#xdf; ein einfacher Aufruf wirklich klappt">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1483748323014" FOLDED="true" ID="ID_1031176077" MODIFIED="1488423308107" TEXT="Nachrichten vom UI-Bus entnehmen">
<icon BUILTIN="button_ok"/>
<node COLOR="#520262" CREATED="1483908814960" FOLDED="true" HGAP="34" ID="ID_430949603" MODIFIED="1488423308093" TEXT="Design sinnvoll?" VSHIFT="6">
<linktarget COLOR="#b694bf" DESTINATION="ID_430949603" ENDARROW="Default" ENDINCLINATION="178;425;" ID="Arrow_ID_1062305149" SOURCE="ID_329609486" STARTARROW="Default" STARTINCLINATION="-629;-2354;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="help"/>
<node CREATED="1483908894494" FOLDED="true" ID="ID_1854211202" MODIFIED="1488423308074" TEXT="Proc-Commands...">
<node CREATED="1483908919274" ID="ID_184360294" MODIFIED="1483908924253" TEXT="k&#xf6;nnte man direkt aufrufen"/>
<node CREATED="1483908982889" ID="ID_758147179" MODIFIED="1483908992252" TEXT="sind bereits ein Handle"/>
</node>
<node CREATED="1483909157290" FOLDED="true" ID="ID_1197073738" MODIFIED="1488423308074" TEXT="via UI-Bus">
<node CREATED="1483909161170" ID="ID_159742986" MODIFIED="1483909170373" TEXT="bleibt ein synchroner Aufruf"/>
<node CREATED="1483909179088" ID="ID_926171934" MODIFIED="1483909221854" TEXT="Command(Handle) auf beiden Seiten"/>
<node CREATED="1483909223058" FOLDED="true" ID="ID_582886428" MODIFIED="1488423308053" TEXT="Argument-Marshalling">
<node CREATED="1483909253550" ID="ID_824110666" MODIFIED="1483909262056" TEXT="umst&#xe4;ndlich, mehrstufig"/>
<node CREATED="1483909263620" ID="ID_1628441605" MODIFIED="1483909277414" TEXT="erzwingt mehrfache Indirektion"/>
<node CREATED="1483909333227" ID="ID_473681935" MODIFIED="1483909347045" TEXT="Umpacken -&gt; Record -&gt; Tuple -&gt; function"/>
</node>
<node CREATED="1483909386820" FOLDED="true" ID="ID_311424247" MODIFIED="1488423308053" TEXT="mehrfache Indirektion">
<node CREATED="1483909440437" ID="ID_1792180060" MODIFIED="1483909458398" TEXT="Variant-Record VTable"/>
<node CREATED="1483909459226" ID="ID_1352346417" MODIFIED="1483909465933" TEXT="Bus-Term VTable"/>
<node CREATED="1483909468913" ID="ID_1209442982" MODIFIED="1483909487842" TEXT="Command-Handler double dispatch"/>
<node CREATED="1483909488622" ID="ID_856721427" MODIFIED="1483909495025" TEXT="Argument-Auswertung double dispatch"/>
<node CREATED="1483909496110" ID="ID_1500666281" MODIFIED="1483909508008" TEXT="Command-Functor invocation VTable"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1483909585362" FOLDED="true" HGAP="1" ID="ID_1047452263" MODIFIED="1488423308075" TEXT="mu&#xdf; sich noch rechtfertigen" VSHIFT="11">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1483909605879" ID="ID_1463483386" MODIFIED="1483909742954" TEXT="war eine Design-Wahl auf Verdacht">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1483909613670" ID="ID_223762457" MODIFIED="1483909705485" TEXT="erzwingt Trennung im Planen und Denken"/>
<node CREATED="1483909679605" ID="ID_1627582396" MODIFIED="1483909688616" TEXT="k&#xf6;nnte Verallgemeinerungen im UI generieren"/>
<node CREATED="1483909714201" ID="ID_1677179961" MODIFIED="1483909722659" TEXT="k&#xf6;nnte UI-Abl&#xe4;ufe abstrakt testbar machen"/>
</node>
</node>
<node CREATED="1483755303243" FOLDED="true" HGAP="86" ID="ID_1425274731" MODIFIED="1488423308093" TEXT="Brauche Command-Handler" VSHIFT="11">
<icon BUILTIN="button_ok"/>
<node CREATED="1483755317105" ID="ID_775818551" MODIFIED="1483755319685" TEXT="ist ein Visitor"/>
<node CREATED="1483755320136" FOLDED="true" ID="ID_1343109260" MODIFIED="1488423308075" TEXT="Knackpunkt: Command-ID">
<node CREATED="1483755331671" FOLDED="true" ID="ID_1913103415" MODIFIED="1488423308053" TEXT="wird im Visitor gebraucht">
<node CREATED="1483909964935" ID="ID_991182613" MODIFIED="1483910009457">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      will sagen: <i>in </i>den Visitor-Methoden-Implementierungen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1483909984005" ID="ID_590392737" MODIFIED="1483910003178">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>aber</i>&#160;ist nicht auf dem Visitor-Interface darstellbar
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1483910035326" FOLDED="true" ID="ID_366990653" MODIFIED="1488423306846" TEXT="latentes Design-Problem">
<icon BUILTIN="idea"/>
<node CREATED="1483910047148" ID="ID_1870862761" MODIFIED="1483910055271" TEXT="Design Rec&lt;GenNode&gt;"/>
<node CREATED="1483910056243" ID="ID_967619584" MODIFIED="1483910089274" TEXT="da Record eine Payload ist"/>
<node CREATED="1483910117819" ID="ID_117939525" MODIFIED="1483910215292" TEXT="man k&#xf6;nnte....">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....&#252;ber einen GenNode-Visitor nachdenken
    </p>
    <p>
      <b>aber</b>
    </p>
    <ul>
      <li>
        nicht jetzt
      </li>
      <li>
        das Problem m&#252;&#223;te mehrfach auftreten
      </li>
      <li>
        k&#246;nnte zu Switch-On-Type-programming f&#252;hren
      </li>
    </ul>
  </body>
</html></richcontent>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
<node CREATED="1483755341814" FOLDED="true" ID="ID_254491480" MODIFIED="1488423308054" TEXT="Instanz-Management fraglich">
<node CREATED="1483909934891" ID="ID_1608078388" MODIFIED="1483909940038" TEXT="nicht wirklich relevant"/>
<node CREATED="1483909940506" ID="ID_262925518" MODIFIED="1483909951669" TEXT="effektiv nur Stack-Storage"/>
</node>
<node CREATED="1483755347949" FOLDED="true" ID="ID_4176562" MODIFIED="1488423308054" TEXT="Performance-&#xdc;berlegungen">
<node CREATED="1483909826690" ID="ID_1903951356" MODIFIED="1483909898026" TEXT="nicht wirklich schlimm">
<icon BUILTIN="smiley-neutral"/>
</node>
<node CREATED="1483909832385" ID="ID_136936347" MODIFIED="1483909836244" TEXT="Storage ist lokal"/>
<node CREATED="1483909836992" ID="ID_238373170" MODIFIED="1483909866352" TEXT="VTable ist vorbereitet"/>
<node CREATED="1483909866844" ID="ID_83144371" MODIFIED="1483909873843" TEXT="aber: double-dispatch">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1483755360196" ID="ID_514688462" MODIFIED="1483755368542" TEXT="Nachrichten kommen ohne bestimmte Ordnung an"/>
</node>
</node>
<node CREATED="1483910254385" FOLDED="true" HGAP="88" ID="ID_267402281" MODIFIED="1488423308093" TEXT="Command-Instanz-Management" VSHIFT="-8">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1483910266799" ID="ID_86909318" MODIFIED="1483910282066" TEXT="wie funktioniert&apos;s?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1483910289396" ID="ID_1014786685" MODIFIED="1483910293311" TEXT="Prototyp-Pattern"/>
<node CREATED="1483910293827" FOLDED="true" ID="ID_1957585594" MODIFIED="1488423308075" TEXT="irgendjemand...">
<node CREATED="1483910333137" ID="ID_1304876612" MODIFIED="1483910340745" TEXT="mu&#xdf; Command-Definitionen Klonen"/>
<node CREATED="1483910306458" ID="ID_601011326" MODIFIED="1483910315557" TEXT="und dabei IDs generieren / dekorieren"/>
</node>
<node CREATED="1483911060894" FOLDED="true" ID="ID_1204552049" MODIFIED="1488423308075" TEXT="wer">
<icon BUILTIN="help"/>
<node CREATED="1483911069277" FOLDED="true" ID="ID_277421164" MODIFIED="1488423308054" TEXT="InteractionStateManager">
<node CREATED="1483911145898" ID="ID_897832170" MODIFIED="1483911160684" TEXT="eigentlich nur opaque Command-IDs"/>
<node CREATED="1483911161248" ID="ID_283380145" MODIFIED="1483911176626" TEXT="aber: wei&#xdf; wann neue Interaktion beginnt"/>
</node>
<node CREATED="1483911091314" FOLDED="true" ID="ID_842914123" MODIFIED="1488423308054" TEXT="SessionCommandService">
<node CREATED="1483911181669" ID="ID_1397857321" MODIFIED="1483911193568" TEXT="sitzt schon in Proc"/>
<node CREATED="1483911194940" ID="ID_485403998" MODIFIED="1483911207598" TEXT="&quot;nat&#xfc;rlich&quot; nah am Command-Framework"/>
<node CREATED="1483911208546" ID="ID_380202969" MODIFIED="1483911221076" TEXT="wei&#xdf; aber nichts von Interaktionen"/>
</node>
</node>
<node CREATED="1483921000340" FOLDED="true" HGAP="-60" ID="ID_1814046433" MODIFIED="1488423308075" VSHIFT="31">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>Idee:</i>&#160;Zusammenarbeit
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1483921070115" ID="ID_398046799" MODIFIED="1483921131646" TEXT="Prototyp-Service"/>
<node CREATED="1483921132114" ID="ID_1652398711" MODIFIED="1483921138070" TEXT="liefert dekorierte Command-ID"/>
<node CREATED="1483921149368" ID="ID_1028150245" MODIFIED="1483921160762" TEXT="im GUI nur noch Command-IDs"/>
<node CREATED="1483924872930" ID="ID_242219866" MODIFIED="1483925475104">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das w&#228;re eine <b>Protokoll-Erweiterung</b>
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#e0ac6d" DESTINATION="ID_242219866" ENDARROW="Default" ENDINCLINATION="-2132;93;" ID="Arrow_ID_1940592535" SOURCE="ID_876058329" STARTARROW="Default" STARTINCLINATION="2588;-604;"/>
<linktarget COLOR="#e9ba2d" DESTINATION="ID_242219866" ENDARROW="Default" ENDINCLINATION="-784;1692;" ID="Arrow_ID_1617518648" SOURCE="ID_1479669922" STARTARROW="None" STARTINCLINATION="3436;-1136;"/>
</node>
<node CREATED="1483927472385" HGAP="30" ID="ID_774425061" MODIFIED="1483927503424" VSHIFT="13">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vertagt; Ticket <font color="#c52a05">#1058</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="bell"/>
</node>
</node>
</node>
<node CREATED="1483755390640" FOLDED="true" ID="ID_1882657994" MODIFIED="1488423308093" TEXT="Vorlage">
<icon BUILTIN="info"/>
<node CREATED="1483755399334" FOLDED="true" ID="ID_582834462" MODIFIED="1488423308075" TEXT="AbstractTangible_test">
<node CREATED="1483755406254" ID="ID_1800773105" MODIFIED="1483755410665" TEXT="hat inline-Visitor"/>
<node CREATED="1483755445984" ID="ID_468077802" MODIFIED="1483755455255" TEXT="aber Command-ID fest gedrahtet">
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node CREATED="1483755412605" FOLDED="true" ID="ID_220502028" MODIFIED="1488423308075" TEXT="BusTerm_test">
<node CREATED="1483755417932" ID="ID_1696122428" MODIFIED="1483755426679" TEXT="verwendet Hanlder vom Test-Nexus"/>
<node CREATED="1483755428354" ID="ID_1619772942" MODIFIED="1483755442790" TEXT="der verwendet bereits komplettes Argument-Binding">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1483924890656" FOLDED="true" HGAP="-45" ID="ID_815841886" MODIFIED="1488423308093" TEXT="Implementierung" VSHIFT="22">
<icon BUILTIN="button_ok"/>
<node CREATED="1483924900583" ID="ID_12753464" MODIFIED="1483924949597" TEXT="banal">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1483924903190" ID="ID_1436586014" MODIFIED="1483924947483" TEXT="wenn man das Design akzeptiert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t,
    </p>
    <p>
      die mehrfachen Indirektionen und das Ein-/Auspacken der Argumente
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node COLOR="#338800" CREATED="1483927552823" FOLDED="true" HGAP="-105" ID="ID_458362835" MODIFIED="1488423308094" TEXT="Unit-Test" VSHIFT="12">
<icon BUILTIN="button_ok"/>
<node CREATED="1484294389310" ID="ID_100306048" MODIFIED="1484294425156" TEXT="direkter Aufruf der Facade">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484294397597" ID="ID_1461153044" MODIFIED="1484294422192" TEXT="Aufruf &#xfc;ber Nachrichten + Visitor">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484294409827" FOLDED="true" ID="ID_1014781174" MODIFIED="1488423308075" TEXT="massiv-paralleler Aufruf">
<icon BUILTIN="button_ok"/>
<node CREATED="1484377327032" ID="ID_1461940140" MODIFIED="1484377335042" TEXT="war eine interessante &#xdc;bung...">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1484377336983" ID="ID_823071776" MODIFIED="1484377354528" TEXT="Hilfsmittel f&#xfc;r Thread/Familien-IDs">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1484377358324" FOLDED="true" ID="ID_1280997474" MODIFIED="1488423308054" TEXT="in der Tat">
<icon BUILTIN="idea"/>
<node CREATED="1484377383249" ID="ID_1422297147" MODIFIED="1484377388516" TEXT="Commands &#xfc;berholen sich"/>
<node CREATED="1484377389056" ID="ID_930164551" MODIFIED="1484377408390" TEXT="Command instanzen d&#xfc;rfen nicht geshared werden">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1483748340524" ID="ID_841368846" MODIFIED="1484377283110" TEXT="API-Funktionen aufrufen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1482464658193" FOLDED="true" HGAP="-14" ID="ID_726798260" MODIFIED="1490986126286" TEXT="Ausgestaltung" VSHIFT="29">
<icon BUILTIN="button_ok"/>
<node CREATED="1482465840843" FOLDED="true" ID="ID_818984499" MODIFIED="1488423308107" TEXT="Entwurf">
<icon BUILTIN="stop-sign"/>
<node CREATED="1482464676366" FOLDED="true" ID="ID_1064506750" MODIFIED="1488423308094" TEXT="bekommt Nachrichten vom UI-Bus">
<node CREATED="1482464755404" ID="ID_213932356" MODIFIED="1482464757487" TEXT="Argument"/>
<node CREATED="1482464758148" ID="ID_545671857" MODIFIED="1482464759104" TEXT="Bang"/>
</node>
<node CREATED="1482464696668" ID="ID_1191503480" MODIFIED="1482464706174" TEXT="m&#xfc;ssen in Command-Objekte &#xfc;bersetzt werden"/>
<node CREATED="1482464712474" ID="ID_1386285986" MODIFIED="1482464718917" TEXT="Interface sollte noch GenNodes nehmen"/>
<node CREATED="1482464719513" ID="ID_1003624750" MODIFIED="1482464729939" TEXT="aber synchron die Commands beziehen"/>
<node CREATED="1482464730783" ID="ID_1225624791" MODIFIED="1482464740714" TEXT="damit Exceptions sofort fliegen"/>
</node>
<node CREATED="1484377470133" FOLDED="true" HGAP="35" ID="ID_62981197" MODIFIED="1488423308108" TEXT="Weiterungen" VSHIFT="11">
<icon BUILTIN="hourglass"/>
<node CREATED="1484377485339" ID="ID_414407137" MODIFIED="1484377497877" TEXT="Commands mit verschiedener Priorit&#xe4;t"/>
<node CREATED="1484377503465" ID="ID_72812588" MODIFIED="1484377510243" TEXT="Commands aggregieren"/>
<node CREATED="1484377535829" ID="ID_488242691" MODIFIED="1484377545519" TEXT="Verwerfen von Commands"/>
<node CREATED="1484377546779" ID="ID_1097356179" MODIFIED="1484377552630" TEXT="Command-ID - Typ"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1482465854513" FOLDED="true" HGAP="26" ID="ID_1477532018" MODIFIED="1488423308108" TEXT="Widerspruch zu Tangible " VSHIFT="12">
<icon BUILTIN="broken-line"/>
<node CREATED="1482465965660" ID="ID_1945056449" MODIFIED="1482465965660" TEXT="Tangible::prepareCommand"/>
<node CREATED="1482466012484" ID="ID_283758092" MODIFIED="1482466020782" TEXT="verwendet InvocationTrail"/>
<node CREATED="1482466021562" ID="ID_620393388" MODIFIED="1482466103867" TEXT="dieser wird direkt mit Command erzeugt"/>
<node CREATED="1482466509953" ID="ID_565604124" MODIFIED="1482466586294" TEXT="...dient der Sicherheit">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denn es erzwingt,
    </p>
    <p>
      da&#223; die betreffenden Commands schon erzeugt und registriert sein m&#252;ssen,
    </p>
    <p>
      wenn in der UI ein InvocationTrail angelegt wird.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1482466104359" ID="ID_235553653" MODIFIED="1482466115902" TEXT="speichert aber eigentlich nur die Cmd-ID">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1482466197707" FOLDED="true" ID="ID_832611965" MODIFIED="1488423308108">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Architektur-Entscheidung
    </p>
    <p>
      <i>kann offen bleiben</i>
    </p>
  </body>
</html></richcontent>
<node CREATED="1482466246933" ID="ID_870716233" MODIFIED="1482466256655" TEXT="h&#xe4;ngt von der Verwendung ab"/>
<node CREATED="1482466257027" ID="ID_1491453181" MODIFIED="1482466260134" TEXT="von InvocationTrail"/>
<node CREATED="1482466260627" ID="ID_1708185529" MODIFIED="1482466265989" TEXT="wie Commands im UI gebunden werden"/>
<node CREATED="1482466271713" ID="ID_1519273419" MODIFIED="1482524336076" TEXT="Nur IDs gehen &#xfc;ber die Leitung">
<icon BUILTIN="info"/>
</node>
<node CREATED="1482466587807" ID="ID_135147405" MODIFIED="1482524332833">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      reine ID-Wirtschaft <i>w&#228;re m&#246;glich</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482466285359" FOLDED="true" ID="ID_1077952925" MODIFIED="1488423308094" TEXT="Zugriff auf proc::control::Command is OK">
<icon BUILTIN="yes"/>
<node COLOR="#c0425f" CREATED="1482466320947" ID="ID_431246960" MODIFIED="1482466419650" TEXT="Grundsatz-Entscheidung">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482466328482" ID="ID_1223809722" MODIFIED="1482466411192" TEXT="GUI darf von unteren Layern abh&#xe4;ngen">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1482466347767" ID="ID_755953852" MODIFIED="1482466385464" TEXT="C++ ist das ma&#xdf;gebliche Ausdrucksmedium">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1482466368068" ID="ID_149410251" MODIFIED="1482466388017" TEXT="und nicht irgend ein abstraktes CLI-Binding">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1481320870369" HGAP="17" ID="ID_454465129" MODIFIED="1484882470134" TEXT="Rolle des &quot;Model&quot; kl&#xe4;ren" VSHIFT="20">
<node CREATED="1484882430390" FOLDED="true" HGAP="56" ID="ID_836665320" MODIFIED="1488423342658" TEXT="Diff-Nachrichten" VSHIFT="-23">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1484882437421" ID="ID_1948154354" MODIFIED="1484882453892" TEXT="#1066 concept to pass diff messages">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1484882477368" FOLDED="true" ID="ID_224140586" MODIFIED="1488423342541" TEXT="Probleme">
<node CREATED="1484882480568" ID="ID_1399902588" MODIFIED="1484882485922" TEXT="unbekannter Quelltyp"/>
<node CREATED="1484882487911" ID="ID_651657111" MODIFIED="1484882492745" TEXT="Storage-Management"/>
<node CREATED="1484882493501" ID="ID_935689465" MODIFIED="1484882554013" TEXT="&#xdc;bergabe in den Event-Thread">
<arrowlink COLOR="#b7748e" DESTINATION="ID_31825421" ENDARROW="Default" ENDINCLINATION="560;150;" ID="Arrow_ID_1434676575" STARTARROW="Default" STARTINCLINATION="587;44;"/>
</node>
<node CREATED="1484882506156" ID="ID_1297602514" MODIFIED="1484882512719" TEXT="Callback aus dem Event-Thread"/>
</node>
</node>
</node>
</node>
<node CREATED="1477784829157" ID="ID_1998357180" MODIFIED="1477784838700" TEXT="Icon-Laden modernisieren"/>
<node CREATED="1477784821925" ID="ID_1871474250" MODIFIED="1488423306858" TEXT="Styling aufr&#xe4;umen">
<node CREATED="1477784846162" ID="ID_1164942946" MODIFIED="1477784889994" TEXT="siehe Info zum CssProvider">
<arrowlink DESTINATION="ID_1810760662" ENDARROW="Default" ENDINCLINATION="1846;-61;" ID="Arrow_ID_1610122569" STARTARROW="None" STARTINCLINATION="-227;-646;"/>
</node>
</node>
</node>
<node CREATED="1485126385235" HGAP="38" ID="ID_548720270" MODIFIED="1488419809961" TEXT="top-Level" VSHIFT="11">
<linktarget COLOR="#851358" DESTINATION="ID_548720270" ENDARROW="Default" ENDINCLINATION="-663;-614;" ID="Arrow_ID_325703166" SOURCE="ID_471609909" STARTARROW="None" STARTINCLINATION="1209;488;"/>
<icon BUILTIN="bell"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485126400233" ID="ID_885004423" MODIFIED="1485126409402" TEXT="unsauberes Design">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1485126410424" ID="ID_1158773288" MODIFIED="1485549693700" TEXT="wir haben mehrere Fenster">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485126418431" ID="ID_1145950660" MODIFIED="1485550989113" TEXT="GtkLumiera sollte kein Singleton sein">
<arrowlink COLOR="#80b3ef" DESTINATION="ID_164246989" ENDARROW="Default" ENDINCLINATION="-42;-74;" ID="Arrow_ID_401425747" STARTARROW="None" STARTINCLINATION="348;19;"/>
</node>
<node CREATED="1485549610040" ID="ID_699947133" MODIFIED="1485549690035" TEXT="Aktionen f&#xfc;r jedes top-level Window repliziert">
<icon BUILTIN="pencil"/>
<node CREATED="1485549646739" ID="ID_1714638860" MODIFIED="1485549664604" TEXT="Abh&#xe4;ngigkeit vom Fenster identifizieren"/>
<node CREATED="1485549666424" ID="ID_1909892697" MODIFIED="1485549672163" TEXT="generisch machen"/>
<node CREATED="1485549672943" ID="ID_1716269299" MODIFIED="1485549679298" TEXT="Aktionen in UiManager verschieben"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1485126429445" ID="ID_832156304" MODIFIED="1485126432879" TEXT="aufr&#xe4;umen">
<icon BUILTIN="flag-pink"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485126506699" ID="ID_1776936645" MODIFIED="1485126569343" TEXT="#1067 rearrange GUI Application top-level">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485126573818" ID="ID_206480879" MODIFIED="1485126655278" TEXT="#1032 use gtk::Application instead of gtk::Main">
<linktarget COLOR="#ae1856" DESTINATION="ID_206480879" ENDARROW="Default" ENDINCLINATION="715;0;" ID="Arrow_ID_926482654" SOURCE="ID_392996871" STARTARROW="Default" STARTINCLINATION="134;383;"/>
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485126609878" ID="ID_815018040" MODIFIED="1485126614405" TEXT="#1064 investigate WindowManager lifecycle">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485550950793" FOLDED="true" ID="ID_50100583" MODIFIED="1488423342658" TEXT="#1048 rectify UI Lifecycle">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1485550968230" FOLDED="true" ID="ID_164246989" MODIFIED="1488423342541" TEXT="GtkLumiera darf kein Singleton mehr sein">
<linktarget COLOR="#80b3ef" DESTINATION="ID_164246989" ENDARROW="Default" ENDINCLINATION="-42;-74;" ID="Arrow_ID_401425747" SOURCE="ID_1145950660" STARTARROW="None" STARTINCLINATION="348;19;"/>
<node COLOR="#338800" CREATED="1485561982971" ID="ID_287757293" MODIFIED="1485917472736" TEXT="Blocker entfernen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1485562029644" ID="ID_1022779645" MODIFIED="1485562063560" TEXT="wieder das Problem mit dem BInden der Actions">
<arrowlink COLOR="#a9b4c1" DESTINATION="ID_530209145" ENDARROW="Default" ENDINCLINATION="-17;-195;" ID="Arrow_ID_626063593" STARTARROW="None" STARTINCLINATION="9;270;"/>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1485550992299" FOLDED="true" ID="ID_1685125156" MODIFIED="1488423342541" TEXT="es blocken">
<icon BUILTIN="button_ok"/>
<node CREATED="1485550999762" ID="ID_249139218" MODIFIED="1485917500008" TEXT="WindowList">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1485551007297" ID="ID_1398242341" MODIFIED="1485917502909" TEXT="Project &amp; Controller">
<icon BUILTIN="button_cancel"/>
</node>
</node>
</node>
<node CREATED="1485551018975" FOLDED="true" ID="ID_1865473127" MODIFIED="1488423342658" TEXT="obsoletes Project &amp; Controller">
<node CREATED="1485551030086" ID="ID_1052165402" MODIFIED="1485551034969" TEXT="in GtkLumiera definiert"/>
<node CREATED="1485551035413" ID="ID_1861899005" MODIFIED="1485551056342" TEXT="tats&#xe4;chlich im Panel (Basisklasse) gespeichert"/>
<node CREATED="1485551056834" ID="ID_538186909" MODIFIED="1485551066884" TEXT="tats&#xe4;chlich nur im alten TimelinePannel gebraucht"/>
<node COLOR="#338800" CREATED="1485551067353" FOLDED="true" ID="ID_1684200224" MODIFIED="1488423342541" TEXT="&#xdc;bergangsl&#xf6;sung">
<icon BUILTIN="button_ok"/>
<node CREATED="1485551080687" ID="ID_405147511" MODIFIED="1485551905501" TEXT="als Felder dort">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1485551085454" ID="ID_472031977" MODIFIED="1485551095662" TEXT="ist ohnehin defunkt">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485551096445" ID="ID_258972556" MODIFIED="1485551103416" TEXT="dann Parameter entfernen"/>
</node>
</node>
<node COLOR="#ae1856" CREATED="1485126686643" ID="ID_405470138" MODIFIED="1485126732565" TEXT="sauberer Shutdown">
<arrowlink COLOR="#872666" DESTINATION="ID_286857196" ENDARROW="Default" ENDINCLINATION="272;63;" ID="Arrow_ID_1606936924" STARTARROW="Default" STARTINCLINATION="1461;0;"/>
</node>
</node>
<node COLOR="#338800" CREATED="1485463620403" ID="ID_40172420" MODIFIED="1485463647941" TEXT="zentralen UiManager einf&#xfc;hren">
<arrowlink COLOR="#667b93" DESTINATION="ID_1698853761" ENDARROW="Default" ENDINCLINATION="19;-86;" ID="Arrow_ID_1952755524" STARTARROW="None" STARTINCLINATION="-290;0;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1485126441508" ID="ID_1227132808" MODIFIED="1485454325864" TEXT="GtkLumiera">
<node CREATED="1485454326827" ID="ID_1493501410" MODIFIED="1485454331748" TEXT="Applikation == Lebenszyklus">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="info"/>
</node>
<node CREATED="1485463515929" ID="ID_785456397" MODIFIED="1485463523940" TEXT="f&#xfc;r die Verdrahtung zust&#xe4;ndig"/>
<node CREATED="1485463525736" ID="ID_1608786246" MODIFIED="1485463536587" TEXT="h&#xe4;lt alle zentralen Komponenten"/>
<node CREATED="1485463537111" ID="ID_929585985" MODIFIED="1485463543226" TEXT="wird nie selber direkt angesprochen"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1485917690936" HGAP="93" ID="ID_939419460" MODIFIED="1485917706156" TEXT="im Moment komplett kaputt" VSHIFT="57">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1485126457025" ID="ID_1698853761" MODIFIED="1489460543116" TEXT="ctrl::UiManager">
<linktarget COLOR="#667b93" DESTINATION="ID_1698853761" ENDARROW="Default" ENDINCLINATION="19;-86;" ID="Arrow_ID_1952755524" SOURCE="ID_40172420" STARTARROW="None" STARTINCLINATION="-290;0;"/>
<icon BUILTIN="pencil"/>
<icon BUILTIN="idea"/>
<node CREATED="1485454242368" ID="ID_482639947" MODIFIED="1485454312303" TEXT="Hub f&#xfc;r Framework">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="info"/>
</node>
<node CREATED="1485454284961" ID="ID_940728768" MODIFIED="1485454304731" TEXT="es gibt nur einen">
<icon BUILTIN="yes"/>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485454375917" ID="ID_1935695209" MODIFIED="1485457020772" TEXT="erbt von Gtk::UiManager">
<icon BUILTIN="messagebox_warning"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485457021805" ID="ID_1476863246" MODIFIED="1485457036813" TEXT="Gtk::UiManager ist deprecated">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1485457105010" ID="ID_202831328" MODIFIED="1485457107477" TEXT="neuer Weg">
<node CREATED="1485457108425" ID="ID_152284167" MODIFIED="1485457124586" TEXT="man erzeugt Gio::SimpleActionGroup"/>
<node CREATED="1485457141853" ID="ID_1841602685" MODIFIED="1485457147592" TEXT="man f&#xfc;gt diese dem Fenster hinzu"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485549716225" ID="ID_1584608575" MODIFIED="1485549726513" TEXT="passende Repr&#xe4;sentation hierf&#xfc;r finden">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1485454243767" ID="ID_3699127" MODIFIED="1485454247467" TEXT="konsistentes UI">
<node CREATED="1485454248335" ID="ID_623536019" MODIFIED="1485454252273" TEXT="globales Men&#xfc;"/>
<node CREATED="1485454259637" ID="ID_432211008" MODIFIED="1485454262840" TEXT="globale Toolbar"/>
<node CREATED="1485454252789" ID="ID_310049457" MODIFIED="1485454258553" TEXT="globale Aktionen"/>
<node CREATED="1485454263876" ID="ID_543704434" MODIFIED="1485454270615" TEXT="globale Keybindings"/>
</node>
</node>
<node CREATED="1489460861250" HGAP="91" ID="ID_771159819" MODIFIED="1489460927424" TEXT="globaler Kontext" VSHIFT="-26">
<font NAME="SansSerif" SIZE="15"/>
<icon BUILTIN="idea"/>
<icon BUILTIN="pencil"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485549049388" ID="ID_530209145" MODIFIED="1485562057742" TEXT="Problem: Aktionen binden">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_530209145" ENDARROW="Default" ENDINCLINATION="-17;-195;" ID="Arrow_ID_626063593" SOURCE="ID_1022779645" STARTARROW="None" STARTINCLINATION="9;270;"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1485549075929" FOLDED="true" ID="ID_1887127861" MODIFIED="1488423342658" TEXT="spezifische Aktionen">
<node CREATED="1485549803366" FOLDED="true" ID="ID_1969698948" MODIFIED="1488423342541" TEXT="Fenster-Bindung">
<node CREATED="1485549810780" ID="ID_71301392" MODIFIED="1485549822342" TEXT="Pop-Ups brauchen ein Vater-Fenster"/>
<node CREATED="1485549828450" ID="ID_1089795419" MODIFIED="1485917553222">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eigentlich wollen wir <i>&quot;das aktuelle&quot;</i>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#375a84" DESTINATION="ID_1041890737" ENDARROW="Default" ENDINCLINATION="177;-181;" ID="Arrow_ID_373431525" STARTARROW="None" STARTINCLINATION="245;-68;"/>
</node>
</node>
</node>
<node CREATED="1485902627783" HGAP="42" ID="ID_88029282" MODIFIED="1489460444614" VSHIFT="-13">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      L&#246;sung: <i><font color="#27754d">schwebende Bindung</font></i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1485902684454" ID="ID_1435239012" MODIFIED="1485902691584" TEXT="Eventgetriebene Oberfl&#xe4;che"/>
<node CREATED="1485902692556" ID="ID_1729615218" MODIFIED="1485902698503" TEXT="wird einmal Verdrahtet"/>
<node CREATED="1485902722360" ID="ID_1003456982" MODIFIED="1485902735679">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      arbeitet dann <i>freischwebend</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1485902750053" FOLDED="true" ID="ID_34756513" MODIFIED="1488423342541" TEXT="zentrales Bindeglied">
<node CREATED="1485902762284" ID="ID_1542199290" MODIFIED="1485902773701" TEXT="kennt die Applikation"/>
<node CREATED="1485902774201" ID="ID_354078562" MODIFIED="1485902778964" TEXT="kennt den Windowmanager"/>
<node CREATED="1485902783201" ID="ID_609776597" MODIFIED="1485902793986" TEXT="kennt einen Operations-Controller"/>
<node CREATED="1485902809813" ID="ID_648457506" MODIFIED="1485902817775" TEXT="kennt den UI-Bus"/>
<node CREATED="1485902794479" ID="ID_1389010579" MODIFIED="1485902806377" TEXT="und ggffs weitere"/>
</node>
<node CREATED="1485902832698" HGAP="48" ID="ID_1440190469" MODIFIED="1489460786881" TEXT="genau dies ist der InteractionDirector" VSHIFT="14">
<arrowlink COLOR="#798eba" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-218;-590;" ID="Arrow_ID_757867749" STARTARROW="None" STARTINCLINATION="15;121;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485902945474" FOLDED="true" HGAP="-2" ID="ID_269152626" MODIFIED="1488423342541" TEXT="L&#xf6;sungs-Detail: dynamisch koppeln" VSHIFT="19">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...meint:
    </p>
    <p>
      wir m&#252;ssen zur Aufrufzeit einer Aktion
    </p>
    <p>
      an den aktuellen Kontext ankoppeln k&#246;nnen.
    </p>
    <p>
      
    </p>
    <p>
      Das hei&#223;t, der UiManager mu&#223; im Stande sein,
    </p>
    <p>
      diesen &quot;aktuellen Kontext&quot; irgendwo <i>aufzufischen</i>
    </p>
  </body>
</html></richcontent>
<node CREATED="1485903041262" ID="ID_997327941" MODIFIED="1485903044082" TEXT="Fokus"/>
<node COLOR="#338800" CREATED="1485903046613" FOLDED="true" ID="ID_1041890737" MODIFIED="1488423308108" TEXT="aktuelles Fenster">
<linktarget COLOR="#375a84" DESTINATION="ID_1041890737" ENDARROW="Default" ENDINCLINATION="177;-181;" ID="Arrow_ID_373431525" SOURCE="ID_1089795419" STARTARROW="None" STARTINCLINATION="245;-68;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1485903652236" ID="ID_1692258488" MODIFIED="1485903657033" TEXT="WindowList">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485903658300" ID="ID_1048329186" MODIFIED="1485903665054" TEXT="durchsuchen..."/>
<node CREATED="1485904084947" ID="ID_64462533" MODIFIED="1485904087814" TEXT="Gtk::Window"/>
<node CREATED="1485904064949" ID="ID_313273027" MODIFIED="1485904081249" TEXT="property_has_focus">
<icon BUILTIN="help"/>
</node>
<node CREATED="1485904072812" ID="ID_829657223" MODIFIED="1485904083263" TEXT="property_is_active">
<icon BUILTIN="help"/>
</node>
<node CREATED="1487039156100" ID="ID_1966427611" MODIFIED="1487039199394" TEXT="implementiert durch die WindowList">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487039170930" ID="ID_1408455006" MODIFIED="1487039195521" TEXT="Actions wei&#xdf;, da&#xdf; es dort implemieriert ist">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node COLOR="#6979a7" CREATED="1489460657437" HGAP="101" ID="ID_674641985" MODIFIED="1489460701613" TEXT="Einsicht: Problem verschiebt sich" VSHIFT="-20">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="idea"/>
<node CREATED="1489460707774" ID="ID_1580714352" MODIFIED="1489460718184" TEXT="Actions wird ein reiner Binde-Kontext"/>
<node CREATED="1489460736370" ID="ID_794231700" MODIFIED="1489460755587" TEXT="delegiert wird an einen globalen controller"/>
<node CREATED="1489460756271" ID="ID_392157120" MODIFIED="1489460770166">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>dort </i>wird der Kontext aufgegriffen
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node COLOR="#338800" CREATED="1485917582383" FOLDED="true" HGAP="163" ID="ID_394170572" MODIFIED="1490986104297" TEXT="Struktur" VSHIFT="-30">
<icon BUILTIN="button_ok"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485917587927" ID="ID_891876702" MODIFIED="1485917622265" TEXT="wohin kommen jetzt die Funktionen, die gebunden werden">
<icon BUILTIN="help"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485917597429" ID="ID_713875638" MODIFIED="1485917626785" TEXT="auf welche Sub-Komponenten wird referenziert">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486063810921" FOLDED="true" ID="ID_719539281" MODIFIED="1488423342541" TEXT="Zweck">
<icon BUILTIN="info"/>
<node CREATED="1486063816320" ID="ID_151179000" MODIFIED="1486063819332" TEXT="ist ein Builder"/>
<node CREATED="1486063819992" ID="ID_445639565" MODIFIED="1486063826794" TEXT="ist eine Closure"/>
<node CREATED="1486063831790" ID="ID_328966967" MODIFIED="1486063840849" TEXT="mu&#xdf; blo&#xdf; am Leben bleiben"/>
<node CREATED="1486063841293" ID="ID_1675242982" MODIFIED="1486063848831" TEXT="hat nach Init nichts mehr zu tun"/>
</node>
<node CREATED="1486063858283" FOLDED="true" HGAP="42" ID="ID_130675776" MODIFIED="1488423666702" TEXT="ist Impl-Detail" VSHIFT="5">
<node CREATED="1486063886407" ID="ID_1294385692" MODIFIED="1486063896241" TEXT="sollte aus den weitverbreiteten Header-Includes raus"/>
<node CREATED="1486063911188" ID="ID_1391435276" MODIFIED="1487275489524" TEXT="gegenw&#xe4;rtig Member im UiManager">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und der wird in der Tat an vielen Stellen includiert
    </p>
    <p>
      und verwendet, und das ist auch gut so
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1486063950103" FOLDED="true" ID="ID_57649689" MODIFIED="1488423308108" TEXT="zwei Modelle">
<node CREATED="1486063954062" FOLDED="true" ID="ID_1174765379" MODIFIED="1488423308094" TEXT="Actions wird insgesamt ein PImpl">
<icon BUILTIN="button_ok"/>
<node CREATED="1486064137014" ID="ID_926465478" MODIFIED="1486064141569" TEXT="ist einfach umzusetzen"/>
<node CREATED="1486064142037" ID="ID_836065841" MODIFIED="1486064154551" TEXT="f&#xfc;hrt zu einer doppelten Indirektion"/>
<node CREATED="1486064289857" ID="ID_806073517" MODIFIED="1486064322201" TEXT="macht Registrierung der Aktionen einfach"/>
<node CREATED="1486064322909" FOLDED="true" ID="ID_89838152" MODIFIED="1488423308076" TEXT="vermischt die Themen">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1486064333076" ID="ID_622214749" MODIFIED="1486064336887" TEXT="Aufbauen des Men&#xfc;s"/>
<node CREATED="1486064337363" ID="ID_551504727" MODIFIED="1486064345942" TEXT="Closure f&#xfc;r die Einzel-Aktionen"/>
</node>
<node CREATED="1486064454451" ID="ID_744832983" MODIFIED="1486064468525" TEXT="Aber genau diese Vermischun macht das Registrieren einfach"/>
</node>
<node CREATED="1486063968868" FOLDED="true" ID="ID_830826934" MODIFIED="1488423308094" TEXT="Actions hat einen ActionGroup-PImpl">
<icon BUILTIN="button_cancel"/>
<node CREATED="1486063991065" ID="ID_1575759747" MODIFIED="1486064393440" TEXT="man kann von ActionGroup erben">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1486064395051" FOLDED="true" ID="ID_1668165644" MODIFIED="1488423308076" TEXT="Risiko: Umstellung auf Gio::Action">
<node CREATED="1486064404410" ID="ID_1844104863" MODIFIED="1486064408350" TEXT="zwingend erforderlich"/>
<node CREATED="1486064408841" ID="ID_630946690" MODIFIED="1486064429511" TEXT="in GTK-4 gibt es keine Gtk::Action mehr">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1486063996776" FOLDED="true" ID="ID_784656430" MODIFIED="1488423308076" TEXT="ist zwar nicht vorgesehen...">
<node CREATED="1486064082093" ID="ID_1957105401" MODIFIED="1486064085153" TEXT="das ist eindeutig"/>
<node CREATED="1486064085605" ID="ID_614915424" MODIFIED="1486064092928" TEXT="es gibt eine statische create-Funktion"/>
<node CREATED="1486064099915" ID="ID_247043577" MODIFIED="1486064107614" TEXT="und die macht einen Gtk::ref_ptr"/>
<node CREATED="1486064111241" ID="ID_1562595756" MODIFIED="1486064121620" TEXT="AUTSCH: ich will einen unique_ptr"/>
</node>
<node CREATED="1486064004143" FOLDED="true" ID="ID_1825804537" MODIFIED="1488423308076" TEXT="....aber geht">
<icon BUILTIN="help"/>
<node CREATED="1486064188334" ID="ID_1128646481" MODIFIED="1486064191370" TEXT="theoretisch ja"/>
<node CREATED="1486064209092" ID="ID_340165076" MODIFIED="1486064220982" TEXT="aber am Ende will der UiManager einen ref_ptr ActionGroup"/>
<node CREATED="1486064221954" ID="ID_429209345" MODIFIED="1486064232226" TEXT="da sollte man sich besser raushalten">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1486064237832" ID="ID_529050138" MODIFIED="1486064253396" TEXT="w&#xe4;re sch&#xf6;n aber nicht ratsam">
<icon BUILTIN="button_cancel"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1486064478968" FOLDED="true" ID="ID_1610028838" MODIFIED="1488423665448" TEXT="Umbau">
<icon BUILTIN="button_ok"/>
<node CREATED="1486064487215" ID="ID_195778340" MODIFIED="1486067334136" TEXT="Actions wird PImpl">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1486064493614" ID="ID_566562539" MODIFIED="1486067337261" TEXT="Actions wird Header-only">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1486064503229" ID="ID_1200627448" MODIFIED="1486067340766" TEXT="alle wildcard-Includes weg">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1487039062273" FOLDED="true" ID="ID_956599122" MODIFIED="1488423342542" TEXT="Zust&#xe4;ndigkeit">
<icon BUILTIN="yes"/>
<node CREATED="1487039071039" ID="ID_1884426258" MODIFIED="1487039079075" TEXT="Herstellen des Hauptmen&#xfc;s"/>
<node CREATED="1487039079759" ID="ID_471335369" MODIFIED="1487039083905" TEXT="Binden der Aktionen"/>
<node CREATED="1487039084285" ID="ID_555104373" MODIFIED="1487039096040" TEXT="Closure f&#xfc;r die Aktionen"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487039098836" ID="ID_199078620" MODIFIED="1487039106012" TEXT="ggfs: Exceptions fangen">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1487039113306" ID="ID_1405514231" MODIFIED="1488423654953" TEXT="Einbindung">
<icon BUILTIN="button_ok"/>
<node CREATED="1487039118001" ID="ID_244074852" MODIFIED="1487039125444" TEXT="globaler Kontext"/>
<node CREATED="1487039125984" ID="ID_622974182" MODIFIED="1487039133027" TEXT="wei&#xdf;, welche Aktion wohin zu delegieren ist"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488422770096" HGAP="56" ID="ID_53463593" MODIFIED="1489462035367" TEXT="#1085 solve setup of top-level menu actions" VSHIFT="-17">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1488423688763" ID="ID_1950800234" MODIFIED="1488423701821" TEXT="UI-Komponenten">
<node CREATED="1488423702977" ID="ID_292894978" MODIFIED="1488423843843" TEXT="TimelinePane">
<arrowlink COLOR="#a1543e" DESTINATION="ID_1753722831" ENDARROW="Default" ENDINCLINATION="1150;-267;" ID="Arrow_ID_912757548" STARTARROW="None" STARTINCLINATION="496;-77;"/>
</node>
<node CREATED="1488423705729" ID="ID_610872163" MODIFIED="1488423709044" TEXT="AssetPane"/>
<node CREATED="1488423709704" ID="ID_14713871" MODIFIED="1488423712052" TEXT="ViewerPane"/>
</node>
<node CREATED="1488423740012" ID="ID_874330545" MODIFIED="1488423745791" TEXT="Lifecycle-Aktionen">
<node CREATED="1488423747187" ID="ID_1351717245" MODIFIED="1488423755766" TEXT="UI schlie&#xdf;en"/>
</node>
<node CREATED="1488423714584" ID="ID_1439137871" MODIFIED="1488494885123" TEXT="Session-Aktionen">
<icon BUILTIN="button_ok"/>
<node CREATED="1488423760105" ID="ID_1527724040" MODIFIED="1488423762709" TEXT="Speichern">
<node CREATED="1488492373919" ID="ID_1898515568" MODIFIED="1488492382545" TEXT="snapshot-Kommando an Session senden"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488494870418" ID="ID_1929432332" MODIFIED="1488494879041" TEXT="TODO: InvocationTrail">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488566247742" ID="ID_254774965" MODIFIED="1488566261845" TEXT="mu&#xdf; doch die Architktur kl&#xe4;ren">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1488423763585" ID="ID_968726788" MODIFIED="1488423768988" TEXT="Schlie&#xdf;en"/>
<node CREATED="1488494907118" ID="ID_1932633471" MODIFIED="1488494915320" TEXT="Speichern-Als">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488494891600" ID="ID_1996245412" MODIFIED="1488494900886" TEXT="Session-Setup-UI schaffen">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1488492153109" ID="ID_1243726860" MODIFIED="1488492163565">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      generisches <i>&#214;ffnen</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1488423720319" ID="ID_1455353650" MODIFIED="1488423727098" TEXT="Sequence-Aktionen">
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1489461756655" ID="ID_1625364312" MODIFIED="1489461771628" TEXT="neue Sequenz">
<icon BUILTIN="pencil"/>
<node CREATED="1489461773213" ID="ID_344390825" MODIFIED="1489461824472" TEXT="als Treiber verwenden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...um die Entwicklung des Designs zu erzwingen
    </p>
    <p>
      und den Teufelskreis zu durchbrechen!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
<node CREATED="1489461838228" ID="ID_283502486" MODIFIED="1489461843087" TEXT="im InteractionDirector"/>
<node CREATED="1489461843747" ID="ID_1499714490" MODIFIED="1489461922546" TEXT="die Command-ID beziehen">
<icon BUILTIN="forward"/>
<node CREATED="1489461856042" ID="ID_1097689316" MODIFIED="1489461862061" TEXT="bedingt Command-ID"/>
<node CREATED="1489461862937" ID="ID_1649764088" MODIFIED="1489461871843" TEXT="bedingt Command-Definition"/>
<node CREATED="1489461872367" ID="ID_1191906862" MODIFIED="1489461882010" TEXT="bedingt CmdAccessor"/>
<node CREATED="1489461905083" ID="ID_1232900605" MODIFIED="1489461911390" TEXT="bedingt Service-Zugang"/>
</node>
<node CREATED="1489544225984" ID="ID_1436775081" MODIFIED="1489544240309" TEXT="Triggern">
<icon BUILTIN="forward"/>
<node CREATED="1489544261547" ID="ID_1624183619" MODIFIED="1489544266933" TEXT="wer">
<icon BUILTIN="help"/>
<node CREATED="1489544403984" ID="ID_1230750297" MODIFIED="1489544416135" TEXT="&quot;ein UI-Element&quot;">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1489544439187" ID="ID_1231587220" MODIFIED="1489544450590" TEXT="Beispiel: Toolbar-Button"/>
<node CREATED="1489544452034" ID="ID_745673239" MODIFIED="1489544459404" TEXT="Beispiel: Kontextmen&#xfc;-Eintrag"/>
<node CREATED="1489544513577" ID="ID_128032926" MODIFIED="1489544521044" TEXT="Beispiel: Aktion per Keybinding"/>
<node CREATED="1489544524128" ID="ID_1668808893" MODIFIED="1489544534314" TEXT="Beispiel: Signal drag-n-drop"/>
</node>
<node CREATED="1489544268018" ID="ID_452323402" MODIFIED="1489544273089" TEXT="was">
<icon BUILTIN="help"/>
<node CREATED="1489544652414" ID="ID_1351449548" MODIFIED="1489544658199" TEXT="lokal bekannt"/>
<node CREATED="1489544640288" ID="ID_1518540017" MODIFIED="1489544651898" TEXT="fest verdrahtet"/>
<node CREATED="1489544659597" ID="ID_1444251202" MODIFIED="1489544666376" TEXT="ggfs. Auswahl aus kleiner Menge"/>
</node>
<node CREATED="1489544717333" ID="ID_1695877485" MODIFIED="1489544720305" TEXT="falls">
<icon BUILTIN="help"/>
<node CREATED="1489544722261" ID="ID_1263012116" MODIFIED="1489544725320" TEXT="ausf&#xfc;hrbar"/>
<node CREATED="1489544725860" ID="ID_1167910092" MODIFIED="1489544728848" TEXT="d.h. aktiviert"/>
<node CREATED="1489544826383" ID="ID_166753518" MODIFIED="1489544970504" TEXT="zwei verschiedene Effekte">
<icon BUILTIN="idea"/>
<node CREATED="1489544836757" ID="ID_1543872004" MODIFIED="1489544891968" TEXT="Ausf&#xfc;hrung abweisen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...stattdessen einen Fehler-Indikator ausl&#246;sen
    </p>
    <p>
      (Beispiel &quot;in-point fehlt&quot;)
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1489544910395" ID="ID_82086808" MODIFIED="1489544965386" TEXT="enable Action">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das ist eine Reaktion,
    </p>
    <p>
      die von einem managing Ui-Element ausgef&#252;hrt wird,
    </p>
    <p>
      aber von einem externen State-Change getriggert wird
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1489461946997" HGAP="-10" ID="ID_1526466868" MODIFIED="1489462026528" TEXT="direkt von den Lambdas aus delegieren" VSHIFT="7">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wir k&#246;nnen den gr&#246;&#223;ten Teil dieser Einzeiler-Funktionen loswerden,
    </p>
    <p>
      da es nur darum geht, via globalCtx auf den passenden Controller zuzugreifen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485898796393" ID="ID_1217726538" MODIFIED="1485898814419" TEXT="#1069 how to refer to the current window">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485555902731" ID="ID_55451880" MODIFIED="1485555951678" TEXT="#1070 how to bind session commands into UI actions">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1489191725382" ID="ID_1332782754" MODIFIED="1489191731742" TEXT="gr&#xf6;&#xdf;eres Thema">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1489191732869" ID="ID_1031180144" MODIFIED="1489191744151" TEXT="nicht sofort zu l&#xf6;sen"/>
<node CREATED="1489191744764" ID="ID_898481930" MODIFIED="1489191839053" TEXT="aber Grundlagen legen">
<arrowlink COLOR="#606d94" DESTINATION="ID_346158251" ENDARROW="Default" ENDINCLINATION="842;21;" ID="Arrow_ID_726918944" STARTARROW="None" STARTINCLINATION="1012;0;"/>
<icon BUILTIN="yes"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485549081728" ID="ID_1122969153" MODIFIED="1488672334405" TEXT="globale Aktionen">
<arrowlink COLOR="#824f68" DESTINATION="ID_826011549" ENDARROW="Default" ENDINCLINATION="-906;-1187;" ID="Arrow_ID_1373278707" STARTARROW="Default" STARTINCLINATION="870;255;"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1488419973349" ID="ID_1855791702" MODIFIED="1488419978923" TEXT="brauche ein Konzept">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1488419979868" ID="ID_474441211" MODIFIED="1488423639484" TEXT="InteractionDirector">
<arrowlink COLOR="#5e4463" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-57;-739;" ID="Arrow_ID_1847127764" STARTARROW="None" STARTINCLINATION="32;89;"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="forward"/>
<node CREATED="1488419993090" ID="ID_1039124749" MODIFIED="1488420026690" TEXT="wird Dreh- und Angelpunkt">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1488420003752" ID="ID_1927319278" MODIFIED="1488420031086" TEXT="l&#xf6;st auch das Model-Root-Problem">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1488420014959" ID="ID_1816556929" MODIFIED="1488420023146" TEXT="war insofern sinnvoller Umweg">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node CREATED="1485909456513" HGAP="55" ID="ID_576215675" MODIFIED="1488494847859" TEXT="top-level-Verdrahtung" VSHIFT="1">
<arrowlink COLOR="#8c95b2" DESTINATION="ID_36527267" ENDARROW="Default" ENDINCLINATION="129;-562;" ID="Arrow_ID_1284182756" STARTARROW="None" STARTINCLINATION="948;154;"/>
<node CREATED="1485909560027" ID="ID_1949656657" MODIFIED="1487039010986" TEXT="baut den UI-Rahen auf"/>
<node CREATED="1485909491412" ID="ID_1205077540" MODIFIED="1487039041806" TEXT="macht die Applikation betriebsbereit">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1486942467620" FOLDED="true" HGAP="151" ID="ID_1761000392" MODIFIED="1488494806427" TEXT="Alternativen" VSHIFT="14">
<icon BUILTIN="help"/>
<node CREATED="1486942493208" FOLDED="true" ID="ID_805305597" MODIFIED="1488423342542" TEXT="alles via InteractionDirector">
<icon BUILTIN="button_cancel"/>
<node CREATED="1486944032334" ID="ID_1959218653" MODIFIED="1486944046824" TEXT="er macht ohnehin fast alles">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1486944056595" FOLDED="true" ID="ID_423413011" MODIFIED="1488423308108" TEXT="Actions wird eine leere H&#xfc;lle">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1486944064506" ID="ID_712470482" MODIFIED="1486944068493" TEXT="baut das Men&#xfc;"/>
<node CREATED="1486944068985" ID="ID_1074291755" MODIFIED="1486944074916" TEXT="stellt die Closures bereit"/>
<node CREATED="1486944075456" ID="ID_1851635277" MODIFIED="1486944079556" TEXT="k&#xf6;nnte Exceptions fangen"/>
<node CREATED="1486944081143" ID="ID_310163833" MODIFIED="1486944114879" TEXT="...ist das wirklich so schlecht?">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1487034056153" FOLDED="true" ID="ID_1455538166" MODIFIED="1488423308108" TEXT="Rein aus dem Bauch heraus....">
<icon BUILTIN="stop-sign"/>
<node CREATED="1487034063408" ID="ID_1251347399" MODIFIED="1487034078308">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das <i>verschiebt</i>&#160;das Problem nur
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1487034078974" ID="ID_796487731" MODIFIED="1487034094280" TEXT="verletzt das single responsibility pattern"/>
</node>
</node>
<node CREATED="1486942523724" ID="ID_843953045" MODIFIED="1488492879681" TEXT="globalen Kontext einf&#xfc;hren">
<icon BUILTIN="button_ok"/>
<node CREATED="1486942614400" ID="ID_872912875" MODIFIED="1486942627890" TEXT="Action-Definition &quot;wei&#xdf;&quot; wer das konkret machen kann"/>
<node CREATED="1486943522965" ID="ID_244569580" MODIFIED="1488492879682" TEXT="konkret...">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1486943529763" FOLDED="true" ID="ID_186058004" MODIFIED="1488423308094" TEXT="WindowList">
<node CREATED="1486943536354" ID="ID_1414688651" MODIFIED="1486943539365" TEXT="neues Fenster"/>
<node CREATED="1486943539809" ID="ID_443462243" MODIFIED="1486943543509" TEXT="Fenster schlie&#xdf;en"/>
<node CREATED="1486943765530" ID="ID_1741627821" MODIFIED="1486943771717" TEXT="show Panel xyz"/>
<node CREATED="1486943555279" ID="ID_1864984187" MODIFIED="1486943582343" TEXT="m&#xf6;glicherweise: gehe zu"/>
</node>
<node CREATED="1486943607120" FOLDED="true" ID="ID_372751802" MODIFIED="1488423308094" TEXT="InteractionDirector">
<node CREATED="1486943922541" ID="ID_1681514542" MODIFIED="1486943928680" TEXT="Quit Lumiera"/>
<node CREATED="1486943614039" ID="ID_1857002831" MODIFIED="1486943628425" TEXT="Session schlie&#xdf;en/&#xf6;ffnen/speichern"/>
<node CREATED="1486943634229" FOLDED="true" ID="ID_423286018" MODIFIED="1488423308076" TEXT="Einstieg in...">
<node CREATED="1486943640004" ID="ID_1474901789" MODIFIED="1486943642783" TEXT="Preferences"/>
<node CREATED="1486943643179" ID="ID_1263689486" MODIFIED="1486943645031" TEXT="Assets"/>
<node CREATED="1486943645554" ID="ID_357680575" MODIFIED="1486943647095" TEXT="Timeline"/>
</node>
<node CREATED="1486943674287" FOLDED="true" ID="ID_1380296040" MODIFIED="1488423308076" TEXT="kontextabh&#xe4;ngig">
<node CREATED="1486943681046" ID="ID_1415550100" MODIFIED="1486943682634" TEXT="Render"/>
<node CREATED="1486943683198" ID="ID_510577218" MODIFIED="1486943690377" TEXT="add Sequence"/>
<node CREATED="1486943691228" ID="ID_684917844" MODIFIED="1486943693696" TEXT="add Track"/>
<node CREATED="1486943854214" ID="ID_382166589" MODIFIED="1486943901435" TEXT="new Viewer"/>
</node>
</node>
<node CREATED="1486943969999" ID="ID_499822898" MODIFIED="1488492879682" TEXT="Meta-Belange">
<node CREATED="1486943974590" ID="ID_233678784" MODIFIED="1486943976585" TEXT="Hilfe"/>
<node CREATED="1486944977005" ID="ID_1548991554" MODIFIED="1486944985992" TEXT="Wizzards"/>
<node CREATED="1486943976942" FOLDED="true" ID="ID_1215823743" MODIFIED="1488423308076" TEXT="Navigation">
<node CREATED="1486944958215" ID="ID_175734210" MODIFIED="1486944963905" TEXT="ist wohl ein eigener Controller"/>
<node CREATED="1486944947297" ID="ID_572583322" MODIFIED="1486944957556" TEXT="h&#xe4;ngt aber mit InteractionDirector zusammen"/>
<node CREATED="1486944966679" ID="ID_185137660" MODIFIED="1486944974378" TEXT="k&#xf6;nnte ein Kind von diesem sein...."/>
</node>
</node>
</node>
</node>
<node CREATED="1486942533939" FOLDED="true" ID="ID_1309437882" MODIFIED="1488423342542" TEXT="einzelne Closures explizit">
<icon BUILTIN="button_cancel"/>
<node CREATED="1486942574333" ID="ID_189848820" MODIFIED="1486942580399" TEXT="das l&#xe4;uft auf ein Delegate hinaus"/>
<node CREATED="1486942583053" ID="ID_1367249768" MODIFIED="1486942592343" TEXT="...und das w&#xe4;re der InteractionDirector"/>
</node>
</node>
<node CREATED="1487034156380" FOLDED="true" HGAP="29" ID="ID_1632778987" MODIFIED="1489460849340" TEXT="globaler Kontext" VSHIFT="11">
<icon BUILTIN="yes"/>
<node CREATED="1487034162275" ID="ID_28375182" MODIFIED="1487034171926" TEXT="wenige, eng zusammenarbeitende Objekte"/>
<node CREATED="1487034172393" ID="ID_1167812745" MODIFIED="1487034180237" TEXT="...von denen doch jedes seine Aufgabe hat"/>
<node CREATED="1487034180712" ID="ID_1371598611" MODIFIED="1487034193170" TEXT="und jedes genau wei&#xdf;, was die andern k&#xf6;nnen"/>
</node>
</node>
<node CREATED="1489777673022" HGAP="-43" ID="ID_873069403" MODIFIED="1489777689569" TEXT="Zugang zu Proc-Commands" VSHIFT="34">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1489777702971" HGAP="125" ID="ID_1780068142" MODIFIED="1489777822408" TEXT="Framework mu&#xdf; geschaffen werden" VSHIFT="-42">
<icon BUILTIN="yes"/>
<node COLOR="#338800" CREATED="1489777783120" ID="ID_1439783231" MODIFIED="1489777791514" TEXT="Protokoll f&#xfc;r den Zugang">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1489777837120" ID="ID_466927949" MODIFIED="1489777843659" TEXT="Ort f&#xfc;r Command-Definitionen"/>
<node CREATED="1489777846095" ID="ID_1401258681" MODIFIED="1489778075369" TEXT="Design Proc-Command-Framework vorantreiben">
<arrowlink COLOR="#352c7d" DESTINATION="ID_1622574347" ENDARROW="Default" ENDINCLINATION="39;-3322;" ID="Arrow_ID_1512127407" STARTARROW="None" STARTINCLINATION="2000;0;"/>
<font NAME="SansSerif" SIZE="14"/>
</node>
</node>
<node CREATED="1489777738462" ID="ID_733673713" MODIFIED="1489777747448" TEXT="angebunden an den InteractionDirector"/>
<node CREATED="1489777748748" ID="ID_1098831130" MODIFIED="1489777760070" TEXT="statisches Front-End: CmdAccessor"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1486763185746" HGAP="33" ID="ID_362749694" MODIFIED="1489460786881" TEXT="InteractionDirector" VSHIFT="21">
<linktarget COLOR="#798eba" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-218;-590;" ID="Arrow_ID_757867749" SOURCE="ID_1440190469" STARTARROW="None" STARTINCLINATION="15;121;"/>
<linktarget COLOR="#5e4463" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-57;-739;" ID="Arrow_ID_1847127764" SOURCE="ID_474441211" STARTARROW="None" STARTINCLINATION="32;89;"/>
<linktarget COLOR="#683c5b" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-394;0;" ID="Arrow_ID_1979576517" SOURCE="ID_113005643" STARTARROW="None" STARTINCLINATION="-24;-159;"/>
<linktarget COLOR="#1b3863" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-518;1012;" ID="Arrow_ID_959577029" SOURCE="ID_735655054" STARTARROW="Default" STARTINCLINATION="-376;-568;"/>
<icon BUILTIN="pencil"/>
<node CREATED="1486763268223" ID="ID_199328732" MODIFIED="1486763277392" TEXT="nicht der Meister">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1486763278189" ID="ID_330558344" MODIFIED="1486763285944" TEXT="aber ein zentrales Bindeglied"/>
<node CREATED="1486763287244" ID="ID_1712681910" MODIFIED="1486763291199" TEXT="zwei Welten">
<node CREATED="1486763292028" ID="ID_1224392598" MODIFIED="1486763301742" TEXT="Modell-struktur == Hierarchie"/>
<node CREATED="1486763303066" ID="ID_1274975607" MODIFIED="1486763317596" TEXT="Interaktion == Fokus und Kontext"/>
</node>
<node CREATED="1486763350764" ID="ID_1309124609" MODIFIED="1488420106546" TEXT="Aufgaben">
<icon BUILTIN="yes"/>
<node CREATED="1486763355739" ID="ID_745725927" MODIFIED="1486763370261" TEXT="Bindung f&#xfc;r Aktionen bereitstellen"/>
<node CREATED="1488566356774" ID="ID_969011841" MODIFIED="1488672817699" TEXT="Ankerpunkt zum Bilden der Commands">
<arrowlink COLOR="#706a9a" DESTINATION="ID_1461875887" ENDARROW="Default" ENDINCLINATION="-923;-789;" ID="Arrow_ID_457540370" STARTARROW="None" STARTINCLINATION="806;300;"/>
<node CREATED="1488566405248" ID="ID_355508260" MODIFIED="1488566425745" TEXT="auffinden der Command-IDs">
<node CREATED="1489191528425" ID="ID_1331796226" MODIFIED="1489191655977" TEXT="Neuer Service hierf&#xfc;r in Proc">
<arrowlink COLOR="#584d79" DESTINATION="ID_341428100" ENDARROW="Default" ENDINCLINATION="1995;-1317;" ID="Arrow_ID_879741123" STARTARROW="None" STARTINCLINATION="334;-13;"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488677809297" ID="ID_1520540731" MODIFIED="1488677835595" TEXT="Problem: Zugang / Wiring">
<arrowlink DESTINATION="ID_850989325" ENDARROW="Default" ENDINCLINATION="183;-575;" ID="Arrow_ID_1002016515" STARTARROW="None" STARTINCLINATION="341;286;"/>
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1488566427445" ID="ID_567418101" MODIFIED="1488566441831" TEXT="InteractionStateManager">
<node CREATED="1488566447466" ID="ID_1978593319" MODIFIED="1488566452134" TEXT="h&#xe4;ngen darunter"/>
<node CREATED="1488566452538" ID="ID_1569803797" MODIFIED="1488566471539" TEXT="werden transient gebilet"/>
</node>
<node CREATED="1488566472583" ID="ID_1307397775" MODIFIED="1488566481305" TEXT="InvocationTrail beziehen"/>
</node>
<node CREATED="1486763376960" ID="ID_1618020758" MODIFIED="1486763385091" TEXT="Zugang zu den Timelines"/>
<node CREATED="1486763385591" ID="ID_6651258" MODIFIED="1486763391202" TEXT="Zugang zum Asset-Management"/>
<node CREATED="1486763391862" ID="ID_1568245541" MODIFIED="1486763397977" TEXT="Zugang zur Konfiguration"/>
<node CREATED="1486763408780" ID="ID_520762274" MODIFIED="1486763417950" TEXT="Zugang zum persistenten Interface-State"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1486768073223" HGAP="38" ID="ID_1457876217" MODIFIED="1488566292402" TEXT="konkret" VSHIFT="16">
<icon BUILTIN="flag-pink"/>
<node CREATED="1486768080502" ID="ID_1759740464" MODIFIED="1487039233982" TEXT="Binde-Kontext f&#xfc;r globale Aktionen"/>
<node CREATED="1486768086869" ID="ID_173538981" MODIFIED="1488566299019" TEXT="Idee/Konzept f&#xfc;r kontextabh&#xe4;ngige Aktionen">
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1487119208622" FOLDED="true" ID="ID_728171298" MODIFIED="1489454868257" TEXT="Analyse anhand einiger Beispiele">
<icon BUILTIN="pencil"/>
<node CREATED="1488419440935" ID="ID_802346253" MODIFIED="1488419449712" TEXT="add Sequence">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1488419450812" ID="ID_179687468" MODIFIED="1488419456739" TEXT="add Track">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1487119230659" ID="ID_651577067" MODIFIED="1488566303378">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Frage: wieviel Interaction Control
    </p>
    <p>
      m&#252;ssen wir sofort jetzt implementieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487208182655" ID="ID_1220225827" MODIFIED="1487214021907">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      brauche ein <i>aktuelles</i>&#160;Modell-Element
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487208211131" ID="ID_31351462" MODIFIED="1487214024007" TEXT="...und ein Konzept, wie man das gewinnen kann">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487208260557" ID="ID_1057793058" MODIFIED="1487214025311" TEXT="und wer daf&#xfc;r zust&#xe4;ndig sein wird">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
</node>
<node CREATED="1488672665626" ID="ID_590886664" MODIFIED="1488672675810" TEXT="Grundlagen f&#xfc;r Command-handling"/>
<node CREATED="1487313769425" FOLDED="true" ID="ID_728232011" MODIFIED="1488423342542" TEXT="Grundlagen f&#xfc;r InteractionControl">
<node CREATED="1488419854029" ID="ID_932507511" MODIFIED="1488419868604" TEXT="erst mal: Konzept">
<icon BUILTIN="help"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487313191100" FOLDED="true" ID="ID_1619021408" MODIFIED="1488423308109" TEXT="Fazit(TODO)">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1487313203254" FOLDED="true" ID="ID_573801539" MODIFIED="1488423308094" TEXT="Klassen anlegen">
<icon BUILTIN="button_ok"/>
<node CREATED="1487313231355" ID="ID_1566121505" MODIFIED="1487380504988" TEXT="WorkSite">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487313293850" ID="ID_1237669879" MODIFIED="1487380508035" TEXT="WorkSiteTrail">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487313234714" ID="ID_1600517388" MODIFIED="1487380510534" TEXT="SpotLocator">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487313246333" ID="ID_1397075016" MODIFIED="1487380512637" TEXT="FocusTracker">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487313253432" ID="ID_1727250153" MODIFIED="1487380514760" TEXT="Navigator">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487313374879" ID="ID_604164603" MODIFIED="1487380517010" TEXT="UiState">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1487313454220" FOLDED="true" ID="ID_640048374" MODIFIED="1488423308095" TEXT="Abgriff in CoreServices">
<icon BUILTIN="button_ok"/>
<node CREATED="1487313462963" ID="ID_342355040" MODIFIED="1487475672626" TEXT="injizieren">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1487313475593" FOLDED="true" ID="ID_951423078" MODIFIED="1488423308076" TEXT="&#xfc;ber Lifecycle nachdenken">
<icon BUILTIN="button_ok"/>
<node CREATED="1487472682881" ID="ID_1575753680" MODIFIED="1487472687124" TEXT="UI-Bus zuerst"/>
<node CREATED="1487472687641" ID="ID_731426548" MODIFIED="1487472699179" TEXT="dann Ui-Manager -&gt; InteractionDirector -&gt; Ui-State"/>
<node CREATED="1487472711453" FOLDED="true" ID="ID_759583421" MODIFIED="1488423308055" TEXT="beim Zerst&#xf6;ren von GtkLumiera">
<node CREATED="1487472720476" ID="ID_1676214656" MODIFIED="1487472730102" TEXT="sollte die Event-Loop bereits beendet sein"/>
<node CREATED="1487472730667" ID="ID_1826692985" MODIFIED="1487472740645" TEXT="k&#xf6;nnen aber noch Fenster existieren"/>
<node CREATED="1487472757439" ID="ID_977443666" MODIFIED="1487475766036">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wird der Link zwischen CoreService und UI-State <i>dangling</i>
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#659ba4" DESTINATION="ID_977443666" ENDARROW="None" ENDINCLINATION="552;26;" ID="Arrow_ID_1335141107" SOURCE="ID_1261936534" STARTARROW="Default" STARTINCLINATION="462;0;"/>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1487472781532" ID="ID_957134114" MODIFIED="1487472800205" TEXT="kurz darauf wird auch CoreService + Nexus zerst&#xf6;rt"/>
</node>
<node CREATED="1487472971330" ID="ID_81364279" MODIFIED="1487472986931" TEXT="grunds&#xe4;tzliches Shutdown-Problem">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1487472987984" FOLDED="true" ID="ID_1443542622" MODIFIED="1488423308055" TEXT="aber harmlos">
<icon BUILTIN="button_ok"/>
<node CREATED="1487472993439" ID="ID_310404379" MODIFIED="1487473004097" TEXT="unter der Annahme, da&#xdf; das UI wirklich single-threaded l&#xe4;uft"/>
<node CREATED="1487473004949" ID="ID_1092623141" MODIFIED="1487473018151" TEXT="wenn wir die Garantie haben, da&#xdf; die Event-Loop nichts mehr macht"/>
<node CREATED="1487473038377" ID="ID_393673106" MODIFIED="1487473076368" TEXT="Policy: kein Tangible darf im dtor mit dem UI-Bus reden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...mit der Ausnahme des Automatismus,
    </p>
    <p>
      der es selbst vom Bus abkoppelt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1487475679257" FOLDED="true" ID="ID_1700311138" MODIFIED="1488423308077" TEXT="wir injizieren anders herum">
<icon BUILTIN="forward"/>
<node CREATED="1487475695910" ID="ID_1341876014" MODIFIED="1487475704865" TEXT="StateManager als sub-Service"/>
<node CREATED="1487475711092" ID="ID_838045301" MODIFIED="1487475719743" TEXT="geh&#xf6;rt CoreService"/>
<node CREATED="1487475720363" ID="ID_693101732" MODIFIED="1487475723182" TEXT="l&#xe4;uft immer"/>
<node CREATED="1487475723771" ID="ID_959927308" MODIFIED="1487475737732" TEXT="wird in UiState injiziert"/>
<node CREATED="1487475738369" ID="ID_1261936534" MODIFIED="1487475782408" TEXT="harmloses Problem beseitigt">
<arrowlink COLOR="#659ba4" DESTINATION="ID_977443666" ENDARROW="None" ENDINCLINATION="552;26;" ID="Arrow_ID_1335141107" STARTARROW="Default" STARTINCLINATION="462;0;"/>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node COLOR="#338800" CREATED="1487475796409" ID="ID_49608905" MODIFIED="1487475804801" TEXT="StateRecorder nun angeschlossen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1487313659680" FOLDED="true" ID="ID_1420397002" MODIFIED="1488423308095" TEXT="FocusTap">
<node CREATED="1487313668423" ID="ID_1248049801" MODIFIED="1487313679321" TEXT="Signal in model::Tangible"/>
<node CREATED="1487313680053" ID="ID_1563882406" MODIFIED="1487313691288" TEXT="Methode zum Installieren"/>
<node CREATED="1487313691644" ID="ID_1797207964" MODIFIED="1487313704558" TEXT="Verbindung zum FocusTracker herstellen (automatisch)"/>
<node CREATED="1487313717368" ID="ID_1638145586" MODIFIED="1487313725195" TEXT="entsprechend auch in WorkplaceWindow"/>
</node>
<node CREATED="1487313566901" FOLDED="true" ID="ID_268630370" MODIFIED="1488423308095" TEXT="Methoden">
<node CREATED="1487313570932" ID="ID_1733821258" MODIFIED="1487313578015" TEXT="getCurrentWorkSite"/>
<node CREATED="1487313578555" FOLDED="true" ID="ID_671600782" MODIFIED="1488423308077" TEXT="getCurrentMO">
<node CREATED="1487313612063" ID="ID_1795028934" MODIFIED="1487313618034" TEXT="ggfs.sinnvollen Dummy hier"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1487461242609" ID="ID_164538059" MODIFIED="1487461245308" TEXT="Struktur">
<node CREATED="1487461304560" ID="ID_349728972" MODIFIED="1488492188626" TEXT="betreibt">
<node CREATED="1487270206369" ID="ID_1888334597" MODIFIED="1487461321289" TEXT="SpotLocator"/>
<node CREATED="1487273437019" ID="ID_1653934212" MODIFIED="1487461312002" TEXT="Navigator"/>
<node CREATED="1487275297855" ID="ID_898365328" MODIFIED="1487461316314" TEXT="FocusTracker"/>
</node>
<node CREATED="1487461353681" ID="ID_1117222917" MODIFIED="1488492194073" TEXT="Modell">
<node CREATED="1487461361688" ID="ID_1787396394" MODIFIED="1488492200352" TEXT="Attrib">
<node CREATED="1487461413145" ID="ID_816848691" MODIFIED="1487461471666" TEXT="ctrl::UiState">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_816848691" ENDARROW="Default" ENDINCLINATION="111;0;" ID="Arrow_ID_792016162" SOURCE="ID_1032977850" STARTARROW="None" STARTINCLINATION="111;0;"/>
</node>
<node CREATED="1488492205990" ID="ID_1793304375" MODIFIED="1488492278296" TEXT="AssetController"/>
</node>
<node CREATED="1488492229939" ID="ID_996547800" MODIFIED="1488492232598" TEXT="Children">
<node CREATED="1488492233826" ID="ID_1743746426" MODIFIED="1488492238493" TEXT="Timelines"/>
</node>
</node>
<node CREATED="1487461436006" ID="ID_1721976078" MODIFIED="1488566321429" TEXT="Kooperation">
<node CREATED="1487461440509" FOLDED="true" ID="ID_1663368282" MODIFIED="1488423342542" TEXT="CoreService">
<node CREATED="1487461456739" ID="ID_1032977850" MODIFIED="1487461471666" TEXT="delegiert UiState">
<arrowlink DESTINATION="ID_816848691" ENDARROW="Default" ENDINCLINATION="111;0;" ID="Arrow_ID_792016162" STARTARROW="None" STARTINCLINATION="111;0;"/>
</node>
</node>
<node CREATED="1487461476736" FOLDED="true" ID="ID_912393863" MODIFIED="1488566326476" TEXT="Actions">
<node CREATED="1487461484935" ID="ID_951148221" MODIFIED="1487461491098" TEXT="Kontext f&#xfc;r globale Aktionen"/>
</node>
</node>
</node>
</node>
<node CREATED="1485126466520" ID="ID_717310004" MODIFIED="1488423306865" TEXT="WindowManager" VSHIFT="34">
<icon BUILTIN="button_cancel"/>
<node CREATED="1485457325724" ID="ID_1954589768" MODIFIED="1485457325724" TEXT="nur f&#xfc;r die Hauptfenster zust&#xe4;ndig"/>
<node CREATED="1485457328043" ID="ID_1787642462" MODIFIED="1485457367904" TEXT="wir haben kein ApplicationWindow">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn das ist das vereinfachte Setup f&#252;r &quot;einfache&quot; Applikationen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485457370310" ID="ID_1931452525" MODIFIED="1485457379864" TEXT="GtkWindow sind v&#xf6;llig ausreichend"/>
<node CREATED="1485546285407" ID="ID_1129769291" MODIFIED="1485546307908">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mu&#223; kein <b>Manager</b>&#160;sein
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
<node CREATED="1485546319434" ID="ID_1592361314" MODIFIED="1485546343174" TEXT="macht nur eine Sache">
<icon BUILTIN="idea"/>
<node CREATED="1485546325993" ID="ID_1335197005" MODIFIED="1485546333460" TEXT="n&#xe4;mlich eine Liste der Fenster verwalten"/>
</node>
<node CREATED="1485546344471" ID="ID_582242471" MODIFIED="1485546463212" TEXT="WindowList">
<arrowlink DESTINATION="ID_298060478" ENDARROW="Default" ENDINCLINATION="25;-40;" ID="Arrow_ID_683571287" STARTARROW="None" STARTINCLINATION="-62;0;"/>
<icon BUILTIN="forward"/>
</node>
</node>
<node CREATED="1485546396952" ID="ID_298060478" MODIFIED="1485546463212" TEXT="WindowList">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_298060478" ENDARROW="Default" ENDINCLINATION="25;-40;" ID="Arrow_ID_683571287" SOURCE="ID_582242471" STARTARROW="None" STARTINCLINATION="-62;0;"/>
<node CREATED="1485546400807" ID="ID_1118405222" MODIFIED="1485546415097" TEXT="macht, was vom gro&#xdf;en Manager &#xfc;brig blieb"/>
<node CREATED="1485546425452" ID="ID_1088557274" MODIFIED="1485546450588" TEXT="wird von GtkLumiera gehalten und verdrahtet"/>
<node CREATED="1485548669665" ID="ID_285796853" MODIFIED="1485548744609" TEXT="Problem: wie erreichbar?">
<icon BUILTIN="pencil"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485548702476" ID="ID_1295550090" MODIFIED="1485917665638" TEXT="Aufgabe: new_window">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1485548713610" ID="ID_1984416789" MODIFIED="1485548737760" TEXT="wird als Action verdrahtet">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1485548727401" ID="ID_1344937285" MODIFIED="1485548731869" TEXT="was ist die Closure">
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485555987832" ID="ID_313705636" MODIFIED="1485555992480" TEXT="Wiring">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1485555999110" ID="ID_376092703" MODIFIED="1485556008921" TEXT="top-level macht GtkLumiera im ctor"/>
<node CREATED="1485556011260" ID="ID_1618308499" MODIFIED="1485556026758" TEXT="backlink zum Manager"/>
<node CREATED="1486943522965" ID="ID_36527267" MODIFIED="1488494847859" TEXT="top-level-Kontext">
<linktarget COLOR="#8c95b2" DESTINATION="ID_36527267" ENDARROW="Default" ENDINCLINATION="129;-562;" ID="Arrow_ID_1284182756" SOURCE="ID_576215675" STARTARROW="None" STARTINCLINATION="948;154;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1488494592496" ID="ID_1586259377" MODIFIED="1488494596905" TEXT="UI-Bus"/>
<node CREATED="1488494579346" ID="ID_1182508289" MODIFIED="1488494603368" TEXT="UiManager"/>
<node CREATED="1486943529763" ID="ID_1188971928" MODIFIED="1488494606592" TEXT="WindowList"/>
<node CREATED="1486943607120" ID="ID_1101577688" MODIFIED="1488494613248" TEXT="InteractionDirector">
<node CREATED="1486943922541" ID="ID_646168152" MODIFIED="1488494623320" TEXT="Entspricht Model Root"/>
<node CREATED="1486943614039" ID="ID_1354424043" MODIFIED="1488494631023" TEXT="AssetController"/>
<node CREATED="1486943634229" FOLDED="true" ID="ID_1217772313" MODIFIED="1488494640070" TEXT="Kinder: Timelines">
<node CREATED="1486943640004" ID="ID_1237613626" MODIFIED="1486943642783" TEXT="Preferences"/>
<node CREATED="1486943643179" ID="ID_463409583" MODIFIED="1486943645031" TEXT="Assets"/>
<node CREATED="1486943645554" ID="ID_1803125679" MODIFIED="1486943647095" TEXT="Timeline"/>
</node>
<node CREATED="1488494646129" HGAP="34" ID="ID_1817005014" MODIFIED="1488494658528" TEXT="InteractionControl" VSHIFT="11">
<icon BUILTIN="forward"/>
<node CREATED="1488494665798" ID="ID_1228864928" MODIFIED="1488494689255" TEXT="SpotLocator"/>
<node CREATED="1488494689843" ID="ID_1519420241" MODIFIED="1488494692239" TEXT="Navigator"/>
<node CREATED="1488494692802" ID="ID_1706374538" MODIFIED="1488494696246" TEXT="FocusTracker"/>
</node>
</node>
<node CREATED="1486943969999" ID="ID_1823284637" MODIFIED="1488494766061" TEXT="Wizzard">
<node CREATED="1486943974590" ID="ID_1603501979" MODIFIED="1488494780491" TEXT="Hilfe (Website?)"/>
<node CREATED="1486944977005" ID="ID_811162889" MODIFIED="1488494773740" TEXT="kontextsensitive Hilfe"/>
<node CREATED="1488494783190" ID="ID_1886731977" MODIFIED="1488494785274" TEXT="Assistenten"/>
<node CREATED="1488494785774" ID="ID_167814846" MODIFIED="1488494786770" TEXT="About"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488422770096" ID="ID_286551569" MODIFIED="1488422863517" TEXT="#1085 solve setup of top-level menu actions">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485556027322" ID="ID_196345177" MODIFIED="1485556069949" TEXT="Problem: Docking panels">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1485556037465" ID="ID_1657490812" MODIFIED="1485556046020" TEXT="brauchen typischerweise eine Querverbindung"/>
<node CREATED="1485556046703" ID="ID_50463095" MODIFIED="1485556055302" TEXT="geht das &#xfc;ber den Bus alleine?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1485556056206" ID="ID_509456873" MODIFIED="1485556067214" TEXT="...sonst mu&#xdf; der PanelManager helfen">
<icon BUILTIN="yes"/>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1488677721565" ID="ID_850989325" MODIFIED="1488940144847" TEXT="Problem: Zugang">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_850989325" ENDARROW="Default" ENDINCLINATION="183;-575;" ID="Arrow_ID_1002016515" SOURCE="ID_1520540731" STARTARROW="None" STARTINCLINATION="341;286;"/>
<linktarget COLOR="#9f5680" DESTINATION="ID_850989325" ENDARROW="Default" ENDINCLINATION="211;189;" ID="Arrow_ID_1007188680" SOURCE="ID_827179653" STARTARROW="None" STARTINCLINATION="912;-974;"/>
<icon BUILTIN="flag-pink"/>
<node CREATED="1488677729388" ID="ID_764102285" MODIFIED="1488677736385" TEXT="grunds&#xe4;tziches Problem">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1488678069565" ID="ID_346104238" MODIFIED="1488678076272" TEXT="explizite Verdrahtung scheidet aus"/>
<node CREATED="1488678077236" ID="ID_1536759032" MODIFIED="1488678099334" TEXT="klassisches Inversion-Of-Control">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1488678106024" ID="ID_1242521430" MODIFIED="1488678110761" TEXT="access by name">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1488677737419" ID="ID_1621219441" MODIFIED="1488677750577" TEXT="einfach gel&#xf6;st f&#xfc;r top-Level">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1488677751537" ID="ID_75445042" MODIFIED="1488677761574" TEXT="schwierig f&#xfc;r Widgets">
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node COLOR="#338800" CREATED="1486520281897" ID="ID_827558797" MODIFIED="1488677772308" TEXT="Problem: zwei Inseln" VSHIFT="16">
<arrowlink COLOR="#464b68" DESTINATION="ID_1847844548" ENDARROW="Default" ENDINCLINATION="35;-168;" ID="Arrow_ID_1483178089" STARTARROW="Default" STARTINCLINATION="-2;95;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1486520289382" ID="ID_1944158920" MODIFIED="1486520302679" TEXT="CoreService + UI-Bus"/>
<node CREATED="1486520303173" ID="ID_306583765" MODIFIED="1486520321580" TEXT="UiManager, Actions etc"/>
<node CREATED="1486521232144" FOLDED="true" HGAP="44" ID="ID_1816242514" MODIFIED="1491009152832" TEXT="Fragen" VSHIFT="16">
<icon BUILTIN="help"/>
<node CREATED="1486521810252" ID="ID_728134375" MODIFIED="1486521858401" TEXT="wer verk&#xf6;rpert den Sesison-Root?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521250790" ID="ID_447470270" MODIFIED="1486521351988" TEXT="haben wir einen globalen Controller?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521269531" ID="ID_1633937799" MODIFIED="1486521348579" TEXT="was ist das Modell des globalen Controllers?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521285069" ID="ID_410780953" MODIFIED="1486521344837" TEXT="wie h&#xe4;ngt das mit persistentem UI-State zusammen?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521317725" ID="ID_1502356505" MODIFIED="1486521332943" TEXT="welche Aktionen l&#xf6;st der globale Controller aus?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521369166" ID="ID_1068454326" MODIFIED="1486521384972" TEXT="wie werden die mit dem Men&#xfc; verbunden?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521449835" ID="ID_236689642" MODIFIED="1486521473727" TEXT="wo h&#xe4;ngt der InvocationStateManager?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521478792" ID="ID_235720883" MODIFIED="1486521491244" TEXT="wie redet man mit dem InvocationStateManager?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521492102" ID="ID_339608490" MODIFIED="1486521506112" TEXT="kann man Fragen via Bus stellen?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521526105" ID="ID_1872187260" MODIFIED="1486521537413" TEXT="nutzen wir das Advice-System?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486521980109" ID="ID_1036025143" MODIFIED="1491009150722" TEXT="wer beendet das UI?">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1486751116251" FOLDED="true" HGAP="140" ID="ID_113005643" MODIFIED="1491009131177" VSHIFT="61">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Br&#252;cke: <i>gemeinsamer</i>&#160;Controller
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#683c5b" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-394;0;" ID="Arrow_ID_1979576517" STARTARROW="None" STARTINCLINATION="-24;-159;"/>
<node CREATED="1486751221990" ID="ID_563286868" MODIFIED="1486751232207" TEXT="ist Bindeglied f&#xfc;r globale Aktionen"/>
<node CREATED="1486751233315" ID="ID_1611855815" MODIFIED="1486751238982" TEXT="verk&#xf6;rpert den Model-Root"/>
<node CREATED="1486751248097" ID="ID_1313868100" MODIFIED="1486751258147" TEXT="ist Vollmitglied am Bus"/>
</node>
</node>
</node>
<node CREATED="1485126481023" ID="ID_943652445" MODIFIED="1485126488201" TEXT="pro Fenster">
<node CREATED="1485126488981" ID="ID_1346100835" MODIFIED="1485126494129" TEXT="WorkspaceWindow"/>
<node CREATED="1485126494621" ID="ID_923407468" MODIFIED="1485126498416" TEXT="PanelManager"/>
</node>
</node>
</node>
<node CREATED="1476376882857" HGAP="193" ID="ID_1420903777" MODIFIED="1477784783964" TEXT="Kern-Elemente" VSHIFT="-34">
<node CREATED="1480639237820" HGAP="-42" ID="ID_138717265" MODIFIED="1480639353634" TEXT="Bausteine" VSHIFT="-11">
<node CREATED="1480639254498" ID="ID_601165955" MODIFIED="1480639257302" TEXT="Placement"/>
<node CREATED="1480639279591" ID="ID_887618627" MODIFIED="1480639281395" TEXT="Timecode"/>
<node CREATED="1480639276064" ID="ID_1418536459" MODIFIED="1480639277747" TEXT="Fader"/>
<node CREATED="1480639258169" ID="ID_1876553930" MODIFIED="1480639259909" TEXT="Label"/>
<node CREATED="1480639267600" ID="ID_671046048" MODIFIED="1480639269220" TEXT="Clip"/>
<node CREATED="1480639365715" ID="ID_743867283" MODIFIED="1480639368365" TEXT="Bin"/>
</node>
<node CREATED="1480639327169" HGAP="36" ID="ID_700485676" MODIFIED="1480639350283" TEXT="Assets" VSHIFT="-2"/>
<node CREATED="1476376913589" HGAP="42" ID="ID_1887326939" MODIFIED="1480639340628" TEXT="Timeline" VSHIFT="-17">
<node CREATED="1477599995452" HGAP="10" ID="ID_97568136" MODIFIED="1480694723679" TEXT="Bestandteile" VSHIFT="-4">
<node CREATED="1476377043180" ID="ID_1179709828" MODIFIED="1476377047495" TEXT="Head">
<node CREATED="1476377067729" ID="ID_896843893" MODIFIED="1476377070268" TEXT="Patchbay"/>
<node CREATED="1476377071072" ID="ID_745594295" MODIFIED="1476377074604" TEXT="in-sync with body"/>
</node>
<node CREATED="1476377048043" ID="ID_994927647" MODIFIED="1476377052046" TEXT="Body">
<node CREATED="1476377078839" ID="ID_331565543" MODIFIED="1476377086496" TEXT="the Fork">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1476377087942" ID="ID_26794000" MODIFIED="1476377090498" TEXT="expandable"/>
</node>
<node CREATED="1476377059386" ID="ID_363001687" MODIFIED="1476377064293" TEXT="Control">
<node CREATED="1476377117290" ID="ID_179386424" MODIFIED="1476377119406" TEXT="scolling"/>
<node CREATED="1480694668090" FOLDED="true" ID="ID_792729651" MODIFIED="1488423342660" TEXT="TimelineWidget">
<node CREATED="1480694675624" ID="ID_1512443419" MODIFIED="1480694678756" TEXT="Einstiegspunkt"/>
<node CREATED="1480694679232" ID="ID_1344733448" MODIFIED="1480694705355">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      sets f&#252;r <b>eine feste</b>&#160;session::Timeline
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1480694854057" HGAP="32" ID="ID_577309407" MODIFIED="1488470332841" TEXT="TimelinePane" VSHIFT="7">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1480694860696" ID="ID_1847844548" MODIFIED="1488470343957" TEXT="entspricht dem Model-Root">
<linktarget COLOR="#464b68" DESTINATION="ID_1847844548" ENDARROW="Default" ENDINCLINATION="35;-168;" ID="Arrow_ID_1483178089" SOURCE="ID_827558797" STARTARROW="Default" STARTINCLINATION="-2;95;"/>
<icon BUILTIN="yes"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1486750918553" ID="ID_723234072" MODIFIED="1486750929291" TEXT="wirklich??">
<icon BUILTIN="help"/>
</node>
<node CREATED="1486750933252" ID="ID_567923045" MODIFIED="1488470346135" TEXT="sollte doch ehr...">
<icon BUILTIN="idea"/>
<node CREATED="1486750939411" ID="ID_1566629434" MODIFIED="1486750945053" TEXT="ein globaler Controller sein"/>
<node CREATED="1486750945521" ID="ID_759126310" MODIFIED="1486750975987">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      der an <i>geeigneter</i>&#160;Stelle h&#228;ngt
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1486750954906" ID="ID_113736248" MODIFIED="1486750962659" TEXT="und in die TimelinePane injiziert wird"/>
</node>
</node>
<node CREATED="1480694897443" ID="ID_402490324" MODIFIED="1480694916396" TEXT="verwaltet die Timelines"/>
<node CREATED="1480694918056" FOLDED="true" ID="ID_789731200" MODIFIED="1488470342861">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es gibt eine <i>EmptyTimeline</i>
    </p>
  </body>
</html></richcontent>
<node CREATED="1480694930990" ID="ID_1289595423" MODIFIED="1480694936274" TEXT="interface == Widget"/>
</node>
</node>
<node CREATED="1480694624879" HGAP="37" ID="ID_1678741068" MODIFIED="1488470753085" TEXT="TimelineController" VSHIFT="21">
<node CREATED="1480694630302" ID="ID_10965216" MODIFIED="1480694651464" TEXT="hat das Sagen"/>
<node CREATED="1480694652219" ID="ID_1232910852" MODIFIED="1480694656791" TEXT="Widget wird passiv"/>
</node>
</node>
</node>
<node CREATED="1477600006370" ID="ID_1328755612" MODIFIED="1477600008246" TEXT="Ansatz">
<node CREATED="1477600020776" ID="ID_955842824" MODIFIED="1477600023460" TEXT="Einteilung"/>
<node CREATED="1477600027128" ID="ID_1592196047" MODIFIED="1477600035730" TEXT="Handhabung">
<node CREATED="1477600041886" ID="ID_1241593972" MODIFIED="1477600043265" TEXT="User"/>
<node CREATED="1477600044069" FOLDED="true" ID="ID_217967290" MODIFIED="1488423342660" TEXT="technisch">
<node CREATED="1477600126514" FOLDED="true" ID="ID_1433799303" MODIFIED="1488423342543" TEXT="grunds&#xe4;tzlich....">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1477600134217" ID="ID_1352260964" MODIFIED="1487275489756" TEXT="wie viel selber machen?">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Frage ist, wie viel des Verhaltens programmieren wir selber explizit aus,
    </p>
    <p>
      und welchen Teil des Verhaltens &#252;berlassen wir GTK
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1477600216950" FOLDED="true" ID="ID_1935952349" MODIFIED="1488423308109" TEXT="focus">
<node CREATED="1477600231756" FOLDED="true" ID="ID_1744616929" MODIFIED="1488423308095" TEXT="Tabbing">
<node CREATED="1477600258865" ID="ID_112614309" MODIFIED="1477600274354" TEXT="GTK &#xfc;berlassen">
<icon BUILTIN="help"/>
</node>
<node CREATED="1477600275118" ID="ID_1701392398" MODIFIED="1477600284385" TEXT="Ordnung explizit setzen">
<icon BUILTIN="help"/>
</node>
<node CREATED="1477600285685" ID="ID_195435447" MODIFIED="1477600299976" TEXT="effektiv unterbinden">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1477600234036" FOLDED="true" ID="ID_957445675" MODIFIED="1488423308095" TEXT="Navigation">
<node CREATED="1477600335934" ID="ID_1021991791" MODIFIED="1477600342473" TEXT="r&#xe4;umliche Metapher"/>
<node CREATED="1477600353036" ID="ID_529949426" MODIFIED="1477600361398" TEXT="brauche Orts-Repr&#xe4;sentation">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1477600385264" FOLDED="true" ID="ID_734925174" MODIFIED="1488423308109" TEXT="Darstellung">
<node CREATED="1477600397934" ID="ID_224014088" MODIFIED="1477600417763" TEXT="GTK alles geben">
<icon BUILTIN="help"/>
</node>
<node CREATED="1477600402110" ID="ID_375405149" MODIFIED="1477600415503" TEXT="nur eine aktuelle Auswahl">
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node CREATED="1477600431489" FOLDED="true" ID="ID_741336406" MODIFIED="1488423342543" TEXT="gtk::Layout">
<node CREATED="1477600442792" ID="ID_143840935" MODIFIED="1477600450067" TEXT="ist das offizielle Canvas-Control"/>
<node CREATED="1477600450463" ID="ID_293636936" MODIFIED="1477600453523" TEXT="scheint sehr m&#xe4;chtig"/>
<node CREATED="1477600454470" ID="ID_1255426760" MODIFIED="1477600459258" TEXT="ist ein gtk::Container"/>
</node>
</node>
</node>
</node>
<node CREATED="1479678484210" ID="ID_1561971595" MODIFIED="1479678488573" TEXT="Struktur">
<node CREATED="1479678496272" ID="ID_807567871" LINK="file:///home/hiv/devel/lumi/wiki/renderengine.html#GuiTimelineWidgetStructure" MODIFIED="1479678687574" TEXT="Analyse">
<node CREATED="1479678503071" ID="ID_806149006" MODIFIED="1487275489781" TEXT="Grid bringt nix">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Das war zwar schon meine Bauchgef&#252;hl,
    </p>
    <p>
      habe aber sicherheitshalber diese Analyse nochmal gemacht.
    </p>
    <p>
      Details im&#160;&#160;TiddlyWiki....
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1479678634213" ID="ID_432405176" MODIFIED="1479678650821" TEXT="rekursive Struktur schaffen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1479678509822" ID="ID_915690336" MODIFIED="1479678663117" TEXT="Scrolling ist notwendig speziell"/>
</node>
<node CREATED="1479678717961" ID="ID_946163885" MODIFIED="1479678722108" TEXT="globales Widget">
<node CREATED="1479678723576" ID="ID_666987913" MODIFIED="1479678758128" TEXT="Layout Grundmuster: zweigeteilt"/>
<node CREATED="1479678736655" ID="ID_1313901406" MODIFIED="1479678745545" TEXT="globaler Layout-Manager"/>
</node>
<node CREATED="1480723110777" HGAP="24" ID="ID_1197860604" MODIFIED="1480725393020" TEXT="Kontrollstruktur" VSHIFT="7">
<icon BUILTIN="pencil"/>
<node CREATED="1480723136861" FOLDED="true" ID="ID_839909208" MODIFIED="1488470711327" TEXT="Einstiegspunkt">
<arrowlink COLOR="#8d8ba5" DESTINATION="ID_1295897045" ENDARROW="Default" ENDINCLINATION="296;-456;" ID="Arrow_ID_150397161" STARTARROW="None" STARTINCLINATION="1056;0;"/>
<node CREATED="1480723144188" ID="ID_420419439" MODIFIED="1480723148072" TEXT="TimelineWidget"/>
<node CREATED="1480723148572" ID="ID_1331974915" MODIFIED="1480723151671" TEXT="wirklich">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1480723200765" ID="ID_1498754065" MODIFIED="1488470722834" TEXT="aktiv">
<node CREATED="1480723223938" FOLDED="true" ID="ID_1057816659" MODIFIED="1488470731600" TEXT="UI-Mechanik">
<node CREATED="1480723246207" ID="ID_988556037" MODIFIED="1480723268808" TEXT="Scrollen"/>
<node CREATED="1480723270492" ID="ID_1057899601" MODIFIED="1480723272959" TEXT="Zoomen"/>
<node CREATED="1480723288521" FOLDED="true" ID="ID_238022948" MODIFIED="1488423308109" TEXT="Navigieren">
<node CREATED="1480723291697" ID="ID_486282572" MODIFIED="1480723294564" TEXT="delegiert"/>
<node CREATED="1480723296128" ID="ID_681787776" MODIFIED="1480723317025" TEXT="timeline::NavigatorWidget"/>
</node>
<node CREATED="1480723797453" ID="ID_451406554" MODIFIED="1480723801984" TEXT="Benachrichtigungen"/>
</node>
<node CREATED="1480723989315" FOLDED="true" ID="ID_1314277524" MODIFIED="1488470728560" TEXT="zentral">
<node CREATED="1480724166868" FOLDED="true" ID="ID_1338884424" MODIFIED="1488423308109" TEXT="Struktur&#xe4;nderung">
<node CREATED="1480724385214" ID="ID_1913904871" MODIFIED="1480724400259" TEXT="DiffMutable bieten">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1480724171371" FOLDED="true" ID="ID_410013238" MODIFIED="1488423308110" TEXT="Struktur&#xe4;nderung vorbereiten">
<node CREATED="1480724215437" ID="ID_1953691654" MODIFIED="1480724219873" TEXT="Kontextmen&#xfc;"/>
<node CREATED="1480724317120" ID="ID_1898649980" MODIFIED="1480724328098" TEXT="Fokus + Befehl / Taste"/>
<node CREATED="1480724346548" ID="ID_200646693" MODIFIED="1480724360777" TEXT="markieren + Kommando schicken">
<icon BUILTIN="forward"/>
</node>
</node>
</node>
</node>
<node CREATED="1480724417722" FOLDED="true" HGAP="55" ID="ID_940232384" MODIFIED="1488470742663" TEXT="drei M&#xf6;glichkeiten" VSHIFT="5">
<node CREATED="1480724450958" FOLDED="true" ID="ID_493402962" MODIFIED="1488423342543" TEXT="Widget is Tangible">
<icon BUILTIN="button_cancel"/>
<node CREATED="1480724706739" ID="ID_1043502336" MODIFIED="1480724721717" TEXT="kann dann direkt Notifications implementieren"/>
<node CREATED="1480724724585" ID="ID_576219568" MODIFIED="1480724737691" TEXT="mu&#xdf; f&#xfc;r alle Struktur&#xe4;nderungen delegieren"/>
<node CREATED="1480724738583" FOLDED="true" ID="ID_403933116" MODIFIED="1488423308110" TEXT="Achtung: Tangible is noncopyable">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1480724761876" ID="ID_335451126" MODIFIED="1487275489862" TEXT="wegen BusTerm">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      braucht feste Speicher-Addresse
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480724781801" ID="ID_198046255" MODIFIED="1480724795442" TEXT="folglich: nicht in STL-Container">
<icon BUILTIN="stop-sign"/>
</node>
</node>
</node>
<node CREATED="1480724461860" FOLDED="true" ID="ID_865875607" MODIFIED="1488423342543" TEXT="Controller is Tangible">
<icon BUILTIN="button_ok"/>
<node CREATED="1480724851312" ID="ID_1653680733" MODIFIED="1487275489874" TEXT="erzeugt R&#xfc;ckbezug f&#xfc;r Notifications">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ..d.h. der Controller mu&#223; wieder auf das Widget zugreifen
    </p>
    <p>
      und sei es auch blo&#223; &#252;ber ein Interface!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480724910096" ID="ID_874781354" MODIFIED="1480724920595" TEXT="kann direkt das View-Modell manipulieren"/>
<node CREATED="1480724927814" ID="ID_1912287440" MODIFIED="1480724956251" TEXT="ist nicht direkt erreichbar">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1480724935397" ID="ID_1279073903" MODIFIED="1480725166588" TEXT="Modell-Vater mu&#xdf; DiffMutable sehen">
<arrowlink COLOR="#2817aa" DESTINATION="ID_550796340" ENDARROW="Default" ENDINCLINATION="129;0;" ID="Arrow_ID_1199115828" STARTARROW="None" STARTINCLINATION="129;0;"/>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1480724468283" FOLDED="true" ID="ID_1683703083" MODIFIED="1488423342543" TEXT="expose DiffMutable">
<icon BUILTIN="idea"/>
<node CREATED="1480724983774" ID="ID_845297603" MODIFIED="1480724994016" TEXT="hei&#xdf;t: Manipulation au&#xdf;erhalb des Diff-Systems"/>
<node CREATED="1480725000029" ID="ID_1586514211" MODIFIED="1480725010294" TEXT="wir steigen stets auf Ebene einer Timeline ein"/>
<node CREATED="1480725012731" ID="ID_9332776" MODIFIED="1487275489889">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>aber:</i>&#160;Binding im Diff-System durchaus m&#246;glich
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn:
    </p>
    <p>
      das Diff-System verlangt nicht, da&#223; Kinder in der Collection auch Tangible sind.
    </p>
    <p>
      Es verlangt nur
    </p>
    <ul>
      <li>
        da&#223; wir wissen, wie wir Kinder machen
      </li>
      <li>
        da&#223; wir f&#252;r ein gegebenes Kind ein DiffMutable beschaffen k&#246;nnen
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1480725106478" ID="ID_550796340" MODIFIED="1480725166588" TEXT="effektiv auch die L&#xf6;sung f&#xfc;r Alternative-2">
<linktarget COLOR="#2817aa" DESTINATION="ID_550796340" ENDARROW="Default" ENDINCLINATION="129;0;" ID="Arrow_ID_1199115828" SOURCE="ID_1279073903" STARTARROW="None" STARTINCLINATION="129;0;"/>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1479688613990" ID="ID_1537299376" MODIFIED="1479688617705" TEXT="Struktur&#xe4;nderung">
<node CREATED="1479688621637" ID="ID_71591229" MODIFIED="1479688631881" TEXT="notwendig: strukturelles Modell">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1479688633483" ID="ID_1301490505" MODIFIED="1479688646781" TEXT="Elemente in diesem halten einen display-context"/>
<node CREATED="1479688653913" ID="ID_1435784278" MODIFIED="1479688666394" TEXT="dieser wiederum mu&#xdf; f&#xfc;r jede Erweiterung konsultiert werden"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487465759930" HGAP="19" ID="ID_923619509" MODIFIED="1487465825454" VSHIFT="26">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#d71313">Problem</font>: Slave-Timeline
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1487465830224" ID="ID_614775878" MODIFIED="1487465844043" TEXT="mehrere Ansichten f&#xfc;r die gleiche Timeline"/>
<node CREATED="1487465882513" FOLDED="true" ID="ID_347013155" MODIFIED="1488423342661" TEXT="Gr&#xfc;nde">
<icon BUILTIN="info"/>
<node CREATED="1487465847518" ID="ID_1430621359" MODIFIED="1487465854273" TEXT="bewu&#xdf;t verschiedene Ausschnitte"/>
<node CREATED="1487465854653" ID="ID_921534930" MODIFIED="1487465863752" TEXT="unbeabsichtigt in verschiedenen Fenstern"/>
<node CREATED="1487465866499" ID="ID_354960009" MODIFIED="1487465881613" TEXT="Fokussierte Ansicht einer nested sequence"/>
</node>
<node CREATED="1487465918884" FOLDED="true" ID="ID_1775013701" MODIFIED="1488423342661" TEXT="L&#xf6;sungs-Ans&#xe4;tze">
<icon BUILTIN="hourglass"/>
<node CREATED="1487465924572" FOLDED="true" ID="ID_542870958" MODIFIED="1488423342543" TEXT="auf Widget-Ebene doppeln">
<icon BUILTIN="button_cancel"/>
<node CREATED="1487465932554" ID="ID_450673459" MODIFIED="1487465947028" TEXT="greift in die direkte Diff-Mechanik ein"/>
<node CREATED="1487465947504" ID="ID_284663745" MODIFIED="1487465959139" TEXT="Timeline-Komponenten sind selber beliebig komplex"/>
<node CREATED="1487465960198" ID="ID_1257024935" MODIFIED="1487465974352" TEXT="Annahme war immer, da&#xdf; Presenter nahe am Widget arbeitet"/>
</node>
<node CREATED="1487465985315" FOLDED="true" ID="ID_1891341453" MODIFIED="1488423342544" TEXT="Timeline-Controller-Slaves">
<node CREATED="1487465996906" ID="ID_1882876532" MODIFIED="1487465998917" TEXT="machbar"/>
<node CREATED="1487465999417" ID="ID_1034026426" MODIFIED="1487466005172" TEXT="typische Clustering-Probleme"/>
<node CREATED="1487466005952" ID="ID_465457871" MODIFIED="1487466016123" TEXT="Scheitern in-Memory unwahrscheinlich"/>
<node CREATED="1487466021990" ID="ID_212586057" MODIFIED="1487466033369" TEXT="Controller mu&#xdf; aktiv mitwirken"/>
</node>
<node CREATED="1487466040524" FOLDED="true" ID="ID_646231524" MODIFIED="1488423342544" TEXT="Timeline-Slave in der Session">
<node CREATED="1487466064520" ID="ID_1607482571" MODIFIED="1487466076466" TEXT="macht Doppelung explizit"/>
<node CREATED="1487466078743" ID="ID_1134944510" MODIFIED="1487466086321" TEXT="Problem mit den Element-IDs"/>
<node CREATED="1487466090917" ID="ID_1611961848" MODIFIED="1487466099832" TEXT="UI -&gt; Session ist unproblematisch"/>
<node CREATED="1487466100372" FOLDED="true" ID="ID_1630288830" MODIFIED="1488423308110" TEXT="Session/Builder mu&#xdf; aufdoppeln">
<node CREATED="1487466111930" ID="ID_966355925" MODIFIED="1487466114781" TEXT="Diff-Nachrichten"/>
<node CREATED="1487466115154" ID="ID_260455064" MODIFIED="1487466120037" TEXT="Fehler-Markierungen"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487466966478" FOLDED="true" ID="ID_479759046" MODIFIED="1488423342661" TEXT="#1083 solution for Timeline slave">
<icon BUILTIN="bell"/>
<node CREATED="1487466997138" ID="ID_1028434909" MODIFIED="1487467003130" TEXT="auf sp&#xe4;ter vertagt">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1487466986523" ID="ID_39632539" MODIFIED="1487466995787" TEXT="zun&#xe4;chst unterbinden">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1479774700668" HGAP="47" ID="ID_1407821684" MODIFIED="1480725485721" TEXT="Mutation" VSHIFT="13">
<node CREATED="1479774705839" ID="ID_301222108" MODIFIED="1479774729887" TEXT="Problem">
<icon BUILTIN="messagebox_warning"/>
<node COLOR="#d30f0f" CREATED="1479774731141" ID="ID_1523088286" MODIFIED="1479774739456" TEXT="Element != Widget"/>
<node CREATED="1479774747753" ID="ID_279553704" MODIFIED="1479774761996" TEXT="Umordnen der Elemente hat keinen Effekt auf die Anzeige"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1479774764575" ID="ID_1338013643" MODIFIED="1480120324802" TEXT="DiffApplikator geht einfach von STL-Collection aus"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1480120294912" HGAP="31" ID="ID_1734639851" MODIFIED="1488470048093" VSHIFT="23">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      grunds&#228;tzliches
    </p>
    <p>
      Problem
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1480120349873" ID="ID_795629510" MODIFIED="1480120398685" TEXT="Diff repr&#xe4;sentiert &#xc4;nderungen indirekt"/>
<node CREATED="1480120487509" ID="ID_1693545323" MODIFIED="1480120500957">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      speziell die Umordnungen <i>ergeben sich</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480120359191" ID="ID_1039094699" MODIFIED="1480120390087" TEXT="Widget-Container hat keine &#xc4;nderungs-Schnittstelle"/>
<node COLOR="#338800" CREATED="1480725417748" HGAP="28" ID="ID_1996114890" MODIFIED="1480725470323" TEXT="...war dann doch kein so schlimmes Problem" VSHIFT="7">
<arrowlink COLOR="#69ee12" DESTINATION="ID_1964453367" ENDARROW="Default" ENDINCLINATION="-55;-87;" ID="Arrow_ID_636465261" STARTARROW="None" STARTINCLINATION="-116;69;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1480120516442" ID="ID_880374813" MODIFIED="1480120518812" TEXT="Ans&#xe4;tze">
<node CREATED="1480120531935" FOLDED="true" ID="ID_1894474191" MODIFIED="1488423342661" TEXT="Dekorator auf TreeMutator">
<icon BUILTIN="button_cancel"/>
<node CREATED="1480121069464" ID="ID_1707876611" MODIFIED="1480121073803" TEXT="Operationen mitlesen"/>
<node CREATED="1480121074703" ID="ID_1346494338" MODIFIED="1480123649491" TEXT="Notifikations-Schnittstelle">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und der Dekorator w&#252;rde die beobachteten Operationen
    </p>
    <p>
      an diese Notifikations-Schnittstelle senden.
    </p>
    <p>
      Implementiert w&#252;rde sie vom jeweiligen Widget
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480123478013" ID="ID_1281325973" MODIFIED="1480123610437" TEXT="eine Ebene zu tief, aber geht noch">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      korrekt w&#228;re, die Diff-Verben mitzulesen.
    </p>
    <p>
      <i>Das </i>geht aber nicht, weil wir intern (aktiv) iterieren.
    </p>
    <p>
      Wollten wir das doch, m&#252;&#223;ten wir das gesamte Diff-Applikator-Design wegwerfen.
    </p>
    <p>
      
    </p>
    <p>
      Da aber eigentlich eine 1:1-Zuordnung zwischen Diff-Verben und Operations-Primitiven besteht,
    </p>
    <p>
      k&#246;nnte man trotzdem (mit etwas H&#228;ngen und W&#252;rgen) noch hinkommen.
    </p>
    <p>
      Der Dekorator w&#252;rde also auf dem TreeMutator sitzen...
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480121092492" ID="ID_1543810707" MODIFIED="1480123739970" TEXT="L&#xf6;schungen nur heuristisch zu erkennen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Weil wir die &quot;skip&quot;-Operation f&#252;r zwei Zwecke verwenden,
    </p>
    <p>
      und man im Skip nicht wei&#223;, ob man das Element &#252;berhaupt noch anfassen darf,
    </p>
    <p>
      denn es k&#246;nnte ja auch ein von &quot;find&quot; zur&#252;ckgelassener M&#252;ll sein.
    </p>
    <p>
      Daher gibt es die matchSrc-Operation. Effektiv wird die aber nur bei einem Delete aufgerufen...
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480123741362" ID="ID_586646895" MODIFIED="1480123856739" TEXT="unsauber, h&#xe4;sslich, ungl&#xfc;cklich">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <ul>
      <li>
        man sitzt mit dem Detektor unter dem API
      </li>
      <li>
        dadurch entstehen &quot;ungeschriebene Regeln&quot;, wie das API auzurufen ist
      </li>
      <li>
        alternativ k&#246;nnten wir die Operationen komplett 1:1 definieren, also eine explizite delete-Operation einf&#252;hren
      </li>
      <li>
        daf&#252;r w&#252;rde dann die matchSrc wegfallen, was praktisch alle sinnvollen Unit-Tests stark beschr&#228;nkt.
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1480123380843" ID="ID_1717666432" MODIFIED="1480123757244" TEXT="w&#xe4;re prinzipiell machbar">
<icon BUILTIN="forward"/>
</node>
</node>
<node CREATED="1480121177145" FOLDED="true" ID="ID_1964453367" MODIFIED="1488470302801" TEXT="Widget is delegate/slave">
<linktarget COLOR="#69ee12" DESTINATION="ID_1964453367" ENDARROW="Default" ENDINCLINATION="-55;-87;" ID="Arrow_ID_636465261" SOURCE="ID_1996114890" STARTARROW="None" STARTINCLINATION="-116;69;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1480121209277" ID="ID_834494092" MODIFIED="1480123869816" TEXT="Hinzuf&#xfc;gungen erzeugen neue Widgets">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1480121219579" ID="ID_1906821886" MODIFIED="1480123871892" TEXT="L&#xf6;schungen entfernen das zugeh&#xf6;rige Widget">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1480121271493" FOLDED="true" ID="ID_1169797519" MODIFIED="1488470250080" TEXT="Reihenfolge ist nebens&#xe4;chlich">
<icon BUILTIN="idea"/>
<node CREATED="1480123311260" ID="ID_1288896772" MODIFIED="1480123320575" TEXT="Reihenfolge der Widgets == Z-Ordnung"/>
<node CREATED="1480123321707" ID="ID_1842225087" MODIFIED="1480123329269" TEXT="das ist ein reiner UI-Belang"/>
<node CREATED="1480123330521" ID="ID_255024472" MODIFIED="1480123352490" TEXT="Reihenfolge der Tracks wird im UI realisiert"/>
<node CREATED="1480123353030" ID="ID_871730723" MODIFIED="1480123366705" TEXT="Reihenfolge der Clips wird redundant (durch Position) gegeben"/>
</node>
</node>
</node>
<node CREATED="1480123881912" HGAP="6" ID="ID_1550301342" MODIFIED="1480123892997" TEXT="Schlu&#xdf;folgerung" VSHIFT="3">
<icon BUILTIN="yes"/>
<node CREATED="1480123900845" ID="ID_1099759004" MODIFIED="1480123908024" TEXT="wir mutieren ein Modell-Objekt"/>
<node CREATED="1480123908524" ID="ID_599072568" MODIFIED="1480123917999" TEXT="das Widget h&#xe4;ngt an diesem als kompletter Slave"/>
<node CREATED="1480123920371" ID="ID_413127533" MODIFIED="1480123932333" TEXT="Registrierung / Deregistrierung mu&#xdf; vollautomatisch sein"/>
<node CREATED="1480124094867" FOLDED="true" ID="ID_1956638484" MODIFIED="1488470299459">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>nach </i>der Mutation erfolgt <b>Display-Neubewertung</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1480124145476" ID="ID_1268974326" MODIFIED="1480124152519" TEXT="f&#xfc;r einen ganzen Scope"/>
<node CREATED="1480124153979" ID="ID_887707794" MODIFIED="1480124172604" TEXT="stellt fest, was gezeigt werden mu&#xdf;"/>
<node CREATED="1480124174144" ID="ID_1145067443" MODIFIED="1480124210543" TEXT="synthetisiert Anzeige-Parameter (z.B: Koordinaten, Z-Ordnung)"/>
<node CREATED="1480124235128" ID="ID_162610711" MODIFIED="1480124250602" TEXT="mu&#xdf; inkrementell arbeiten und bestehende Widgets anpassen"/>
</node>
</node>
</node>
<node CREATED="1480300573857" HGAP="68" ID="ID_434577605" MODIFIED="1480606887745" TEXT="Anzeige-Steuerung" VSHIFT="39">
<icon BUILTIN="pencil"/>
<node CREATED="1480300584454" ID="ID_202830560" MODIFIED="1480300592344" TEXT="globaler Layout-Manager">
<node CREATED="1480300595820" ID="ID_1004344887" MODIFIED="1480300606631" TEXT="fungiert als Service"/>
<node CREATED="1480300614873" ID="ID_895306481" MODIFIED="1480300625444" TEXT="Ermitteln konkreter Koordinaten"/>
</node>
<node CREATED="1480300627712" ID="ID_1748490788" MODIFIED="1480300640058" TEXT="Display-Manager">
<node CREATED="1480300640790" ID="ID_1461181527" MODIFIED="1480300644825" TEXT="Abstraktion">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1480300645837" ID="ID_337010957" MODIFIED="1480300648544" TEXT="Interface"/>
<node CREATED="1480300649437" ID="ID_704277125" MODIFIED="1480300673402">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      interagiert mit den <b>Presentern</b>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1480300682256" ID="ID_1828380850" MODIFIED="1480300684700" TEXT="Presenter">
<node CREATED="1480300690191" FOLDED="true" ID="ID_1405428387" MODIFIED="1488423342662" TEXT="sind lokales Struktur-Modell">
<node CREATED="1480300733506" ID="ID_186466790" MODIFIED="1480300739877" TEXT="werden per Diff manipuliert"/>
<node CREATED="1480300740537" ID="ID_1322437138" MODIFIED="1487275489978" TEXT="enthalten die Modell-Binding-Daten">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. eine LUID
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1480300699694" ID="ID_1686150984" MODIFIED="1480300712392" TEXT="aktiver Part der View-Steuerung"/>
<node CREATED="1480300717747" ID="ID_1736166807" MODIFIED="1480300724214" TEXT="Master-Slave f&#xfc;r die Widgets"/>
<node CREATED="1480300804840" FOLDED="true" ID="ID_1330942810" MODIFIED="1488423342662" TEXT="Verbindung zum Anzeigekontext">
<node CREATED="1480300822302" ID="ID_1819260908" MODIFIED="1480300833984" TEXT="zwei Kontexte f&#xfc;r die Tracks"/>
<node CREATED="1480300836092" ID="ID_1069009472" MODIFIED="1480300847326" TEXT="Anzeigekontext = Konkretes Vater-Widget"/>
</node>
</node>
</node>
<node CREATED="1480606888860" HGAP="25" ID="ID_899101975" MODIFIED="1480606903105" TEXT="Implementierung" VSHIFT="14">
<icon BUILTIN="pencil"/>
<node CREATED="1480606950372" ID="ID_1502878943" MODIFIED="1480606953527" TEXT="Grundstruktur">
<node CREATED="1480639404070" FOLDED="true" ID="ID_506539007" MODIFIED="1488423342662" TEXT="HeaderPane">
<node CREATED="1480639435010" ID="ID_892799964" MODIFIED="1480639437997" TEXT="Navigator"/>
<node CREATED="1480639438921" FOLDED="true" ID="ID_1668884794" MODIFIED="1488423342544" TEXT="Baum">
<node CREATED="1480639447736" ID="ID_1842963526" MODIFIED="1480639450508" TEXT="Expander"/>
<node CREATED="1480639450960" ID="ID_1468387103" MODIFIED="1480639454795" TEXT="Patchbay"/>
</node>
</node>
<node CREATED="1480639411109" FOLDED="true" ID="ID_520199505" MODIFIED="1488423342662" TEXT="ScrolledPane">
<node CREATED="1480639423116" ID="ID_1226894709" MODIFIED="1480639428735" TEXT="Ruler"/>
<node CREATED="1480639429307" ID="ID_1922498247" MODIFIED="1480639431270" TEXT="Canvas"/>
</node>
</node>
<node CREATED="1480606954115" ID="ID_1178000371" MODIFIED="1480807618481" TEXT="Struktur-Modell">
<icon BUILTIN="button_ok"/>
<node CREATED="1480725215087" FOLDED="true" ID="ID_212845679" MODIFIED="1488423342662" TEXT="erzeugen">
<node CREATED="1480725218783" ID="ID_192239117" MODIFIED="1480725230986" TEXT="Controller ist Tangible">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1480725247115" FOLDED="true" ID="ID_1533101483" MODIFIED="1488423342544" TEXT="Konstruktion erfordert...">
<node CREATED="1480796814716" ID="ID_1742743432" MODIFIED="1487275490012" TEXT="ID einer Timeline">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wir lassen es offen, welche Art von ID das ist.
    </p>
    <p>
      Irgend eine BareEntryID gen&#252;gt
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480725273775" ID="ID_1495656573" MODIFIED="1480725279987" TEXT="Bus-Term-Referenz"/>
</node>
</node>
<node CREATED="1480725377994" FOLDED="true" ID="ID_172881069" MODIFIED="1488423342662" TEXT="verwalten">
<node CREATED="1480741524775" ID="ID_661893589" MODIFIED="1480807588342" TEXT="Bindings einrichten">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1480800574418" FOLDED="true" ID="ID_791426893" MODIFIED="1488423342544" TEXT="Marker">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1480800603331" ID="ID_1846937607" MODIFIED="1480800603331" TEXT="treten in diversen Scopes auf"/>
<node CREATED="1480800643953" ID="ID_409762087" MODIFIED="1480800654307" TEXT="sogar global, auf dem TimeRuler">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1480800605150" ID="ID_1156238228" MODIFIED="1480800608682" TEXT="sind stets optional"/>
<node CREATED="1480807594822" ID="ID_333819085" MODIFIED="1480807600418" TEXT="f&#xfc;hre mal einen Marker-Typ ein"/>
</node>
</node>
</node>
<node CREATED="1480694550601" ID="ID_391329400" MODIFIED="1480694554372" TEXT="TimelineController">
<node CREATED="1480694557112" ID="ID_786517324" MODIFIED="1480694570434" TEXT="Widget ist Startpunkt"/>
<node CREATED="1480694571310" ID="ID_1188786444" MODIFIED="1480694576714" TEXT="aber Controller wird Chef"/>
<node CREATED="1480725633399" FOLDED="true" ID="ID_1475661668" MODIFIED="1488423342662" TEXT="mu&#xdf; erst mal Controller implementieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...abstraktes Interface
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
<node CREATED="1480725680713" ID="ID_448125852" MODIFIED="1480725692188" TEXT="Delegate for Notification">
<icon BUILTIN="help"/>
</node>
<node CREATED="1480725704142" FOLDED="true" ID="ID_992732373" MODIFIED="1488423342544" TEXT="was bleibt abstrakt">
<node CREATED="1480725715916" ID="ID_890987188" MODIFIED="1487275490068" TEXT="buildMutator">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      lat&#252;rnich
    </p>
    <p>
      ...den <i>mu&#223;</i>&#160;jeder individuell implementieren,
    </p>
    <p>
      um die Bindung herzustellen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
</node>
<node CREATED="1480741498930" ID="ID_1605140473" MODIFIED="1480741509309" TEXT="hat nur einen einziten RootTrack">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1480742405505" FOLDED="true" ID="ID_1590367176" MODIFIED="1488423342662" TEXT="ACHTUNG: leere Sequenz?">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>theoretisch</i>&#160;k&#246;nnte man eine Timeline ohne Sequenz
    </p>
    <p>
      oder eine Sequenz ohne root-Fork zulassen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1480776052551" FOLDED="true" ID="ID_431920594" MODIFIED="1488423342544" TEXT="warum">
<node CREATED="1480742420383" ID="ID_19092812" MODIFIED="1480742430313" TEXT="kann ihn nicht gleich im ctor erzeugen"/>
<node CREATED="1480742432749" ID="ID_1840237751" MODIFIED="1480742437344" TEXT="denn wir brauchen die ID"/>
<node CREATED="1480742437892" ID="ID_1335490062" MODIFIED="1480742444383" TEXT="und die kann nur &#xfc;ber den Bus kommen"/>
<node CREATED="1480742445563" ID="ID_1224591813" MODIFIED="1480742457934" TEXT="also m&#xfc;ssen wir warten, bis das Attribut zugewiesen wird"/>
</node>
<node CREATED="1480776067549" FOLDED="true" ID="ID_1500222365" MODIFIED="1488423342544" TEXT="Alternativen">
<node CREATED="1480776081388" FOLDED="true" ID="ID_783764544" MODIFIED="1488423308110" TEXT="via ctor">
<node CREATED="1480776096665" ID="ID_1492275941" MODIFIED="1480776289639" TEXT="das hei&#xdf;t....">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Thema &quot;Darstellung von Objekt-Feldern im Diff&quot;
    </p>
    <p>
      <i>da hab ich mir ausgiebig Gedanken dar&#252;ber gemacht (in dieser Mindmap)</i>
    </p>
    <ul>
      <li>
        entweder ein Feld ist wirklich optional belegbar, dann kann es mit dem Diff kommen
      </li>
      <li>
        wenn dagegen ein Feld zwingend bef&#252;llt sein soll, mu&#223; man das &#252;ber den Konstruktor erzwingen<br />in diesem Fall m&#252;ssen alle Daten bereits mit dem vorangehenden INS kommen,<br />welches den Konstruktor-Aufruf ausl&#246;st<br /><br />
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
<node CREATED="1480776313813" ID="ID_1507386991" MODIFIED="1480776333149" TEXT="Vorteil: einfache Verh&#xe4;ltnisse in der Widget-Impl"/>
<node CREATED="1480776341393" ID="ID_1985821448" MODIFIED="1480776352107" TEXT="Nachteil: mu&#xdf; Diff genau so schicken"/>
<node CREATED="1480777922397" FOLDED="true" ID="ID_514769772" MODIFIED="1488423308095" TEXT="...sonst">
<icon BUILTIN="smily_bad"/>
<node CREATED="1480777941187" ID="ID_1804935890" MODIFIED="1480777949933" TEXT="irgendjemand triggert Laden"/>
<node CREATED="1480777950449" ID="ID_487608965" MODIFIED="1480777963507" TEXT="Core schickt Populations-diff"/>
<node CREATED="1480777972862" ID="ID_1185499319" MODIFIED="1480777978601" TEXT="Exception beim Aufspielen"/>
<node CREATED="1480777979069" ID="ID_1379570617" MODIFIED="1480777982889" TEXT="kann nur geloggt werden"/>
<node CREATED="1480777983381" ID="ID_1125654729" MODIFIED="1480778000894" TEXT="Folglich fehlt im UI die betreffende Timeline"/>
<node CREATED="1480778002098" ID="ID_535894255" MODIFIED="1480778010557" TEXT="sp&#xe4;tere Updates scheitern gleicherma&#xdf;en"/>
<node CREATED="1480778011745" ID="ID_1623403394" MODIFIED="1480778017194" TEXT="dysfunktionales UI">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
<node CREATED="1480776087027" FOLDED="true" ID="ID_132590373" MODIFIED="1488423308110" TEXT="leer zulassen">
<node CREATED="1480778115091" ID="ID_1948130012" MODIFIED="1480778317340" TEXT="das hei&#xdf;t...">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die betreffenden Felder sind echt optional.
    </p>
    <p>
      Der Ctor belegt sie mit einem sinnvollen Leerwert
    </p>
    <p>
      Das Objekt mu&#223; so geschrieben werden, da&#223; es mit den Leerwerten umgehen kann,
    </p>
    <p>
      was typischerweise heit&#223;, da&#223; es verschiedene Betriebsmodi bekommt.
    </p>
    <p>
      
    </p>
    <p>
      Das Diff kann dann sp&#228;ter die konkreten Werte f&#252;r die Attribute nachliefern;
    </p>
    <p>
      typischerweise wird es das in einem Populationsdiff sofort als N&#228;chstes machen.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
<node CREATED="1480778348652" ID="ID_1249761108" MODIFIED="1480778386899" TEXT="Vorteil: Diff-Erzeugung und Aufspielen l&#xe4;uft nach Schema-F"/>
<node CREATED="1480778407052" FOLDED="true" ID="ID_181642617" MODIFIED="1488423308095">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      zwei m&#246;gliche
    </p>
    <p>
      Konsequenzen
    </p>
  </body>
</html></richcontent>
<node CREATED="1480778420418" FOLDED="true" ID="ID_1430286640" MODIFIED="1488423308077" TEXT="Problem ignorieren">
<node CREATED="1480778490545" ID="ID_1228109442" MODIFIED="1480778498347" TEXT="darauf hoffen, da&#xdf; das Diff immer sauber kommt"/>
<node CREATED="1480778499928" ID="ID_1466170472" MODIFIED="1480778510146" TEXT="Felder im Ctor einfach uninitialisiert lassen"/>
<node CREATED="1480778511086" ID="ID_1612105579" MODIFIED="1480778515905" TEXT="keine Checks im Code"/>
<node CREATED="1480778532907" ID="ID_1167492250" MODIFIED="1480778587358">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      funktioniert <i>fast immer</i>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>&quot;was kann denn schon passieren??&quot;</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node CREATED="1480778451502" FOLDED="true" ID="ID_1135553012" MODIFIED="1488423308077" TEXT="echt optionale Felder">
<node CREATED="1480778622039" ID="ID_1007915114" MODIFIED="1480778650859">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Betriebsart <i>&quot;partiell initialisiert&quot;</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480778652819" ID="ID_1335924975" MODIFIED="1480778665661" TEXT="Checks und Verzweigungen im Code"/>
<node CREATED="1480778689191" ID="ID_45333997" MODIFIED="1480778753615" TEXT="Pflanzt sich auf die Nutzer fort">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ..hier das Widget, das ebenfalls
    </p>
    <ul>
      <li>
        nur partiell aufgebaut existieren k&#246;nnen mu&#223;
      </li>
      <li>
        sp&#228;ter sich dynamisch erweitern k&#246;nnen mu&#223;
      </li>
      <li>
        in der Behandlung der UI-Signale ebenfalls checks einbauen mu&#223;
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1480778768988" ID="ID_1385067618" MODIFIED="1480778827398" TEXT="und das alles nur f&#xfc;r....">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      einen Fall, der <i>praktisch nie</i>&#160;auftritt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1480780047561" FOLDED="true" HGAP="88" ID="ID_1993420764" MODIFIED="1488423342544" TEXT="Entscheidung" VSHIFT="16">
<node CREATED="1480780057079" FOLDED="true" ID="ID_481017202" MODIFIED="1488423308110" TEXT="Analyse">
<node CREATED="1480780061327" FOLDED="true" ID="ID_628488180" MODIFIED="1488423308095" TEXT="zwei F&#xe4;lle sind sehr &#xe4;hnlich">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und zwar interessanterweise &#252;ber Kreuz gegliedert
    </p>
    <ul>
      <li>
        die Ctor-L&#246;sung (hat aber etwas mehr Umsetzungsaufwand)
      </li>
      <li>
        die &quot;wird schon klappen&quot;-L&#246;sung
      </li>
    </ul>
  </body>
</html></richcontent>
<node CREATED="1480780163369" ID="ID_851065784" MODIFIED="1480780174083" TEXT="wenig Zusatzaufwand"/>
<node CREATED="1480780174552" ID="ID_640476327" MODIFIED="1480780182891" TEXT="funktioniert fast immer"/>
<node CREATED="1480780183319" ID="ID_431465582" MODIFIED="1480780192913" TEXT="&#xfc;ble Konsequenzen wenn nicht"/>
</node>
<node CREATED="1480780140741" FOLDED="true" ID="ID_1754958261" MODIFIED="1488423308095" TEXT="und ein dritter ist wirklich wasserdicht">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn alle Objekte <i>wirklich</i>&#160;auf partiell initialisierten Zustand vorbereitet sind,
    </p>
    <p>
      und auch &#252;ber ihre APIs dem Nutzer diese Unterscheidnung mit aufzwingen
    </p>
  </body>
</html></richcontent>
<node CREATED="1480780253397" ID="ID_1966427410" MODIFIED="1480780259944" TEXT="zum Preis von deutlich Mehraufwand"/>
<node CREATED="1480780260508" ID="ID_1565963038" MODIFIED="1480780464068" TEXT="plus einer korrodierenden Wirkung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...welche darin besteht,
    </p>
    <p>
      da&#223; man &#252;berall, in der Fl&#228;che, sich um Zustandsabh&#246;ngigkeit k&#252;mmern mu&#223;,
    </p>
    <p>
      und deshalb dazu neigt, das Problem jeweils wegzutricksen.
    </p>
    <p>
      
    </p>
    <p>
      Es besteht also die gro&#223;e Gefahr, zu &quot;s&#252;ndigen&quot; und
    </p>
    <p>
      heimlich in den &quot;wird schon nix passieren&quot; Fall zu geraten.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480780269867" ID="ID_1571900658" MODIFIED="1480780281357" TEXT="daf&#xfc;r kann nie was passieren"/>
</node>
</node>
<node CREATED="1480780494717" FOLDED="true" ID="ID_432076484" MODIFIED="1488423308110" TEXT="Bewertung">
<node CREATED="1480780498437" ID="ID_1806301944" MODIFIED="1480780667751" TEXT="nur die Ctor-L&#xf6;sung ist geradlinig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, nur diese L&#246;sung gr&#252;ndet in der Natur der behandelten Sachverhalte.
    </p>
    <p>
      Wenn etwas seinem Wesen nach nicht optional ist, dann wird es auch nicht optional behandelt.
    </p>
    <p>
      Es ist keine weitere Argumentation notwendig.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480780538975" ID="ID_1139883143" MODIFIED="1480780593840" TEXT="nur die &quot;pa&#xdf;t schon&quot;-L&#xf6;sung ist vern&#xfc;nftig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...nach allen g&#228;ngigen Prinzipien der instrumentellen Vernunft.
    </p>
    <p>
      KISS
    </p>
    <p>
      YAGNI
    </p>
    <p>
      &quot;fokussiere Dich&quot;
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1480780704849" FOLDED="true" ID="ID_483861197" MODIFIED="1488423308110" TEXT="Abw&#xe4;gung">
<node CREATED="1480780715911" ID="ID_1289637688" MODIFIED="1480780729633" TEXT="in der Berufspraxis mu&#xdf; man st&#xe4;ndig ein Auge zudr&#xfc;cken"/>
<node CREATED="1480780731213" ID="ID_773522234" MODIFIED="1480780809290">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hier hab ich <b>endlich mal</b>&#160;die Gelegenheit, sauber zu arbeiten
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480780746851" ID="ID_1128552915" MODIFIED="1480780759173" TEXT="und etwas zu schaffen, f&#xfc;r das ich mich nicht entschuldigen mu&#xdf;"/>
<node CREATED="1480780768272" ID="ID_1174359339" MODIFIED="1480780788674" TEXT="leider f&#xfc;hrt genau diese Haltung dazu, da&#xdf; mir die Helfer davonlaufen"/>
<node CREATED="1480780835319" ID="ID_1263364267" MODIFIED="1480781084576">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hey, es ist <b>mein Leben</b>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...hab ich mich je anders entschieden?
    </p>
    <p>
      <i>wenn</i>&#160;ich mich &#252;berhaupt entscheiden konnte...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1480780930115" HGAP="5" ID="ID_1156407347" MODIFIED="1480781078592" TEXT="L&#xf6;sung: per Konstruktor festlegen" VSHIFT="6">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...nochmal zusammengefa&#223;t
    </p>
    <ul>
      <li>
        immer wenn ein Feld <i>seinem Wesen nach</i>&#160;zwingend gesetzt sein mu&#223; (und aus keinem anderen Grund)
      </li>
      <li>
        dann wird dies per Konstruktor so erzwungen
      </li>
      <li>
        daher mu&#223; dann im Diff bereits im INS-Verb die notwendige Information transportiert werden
      </li>
      <li>
        das hei&#223;t, bei der Diff-Erzeugung mu&#223; man aufpassen und an dieser Stelle bereits einen Record mit den Daten liefern
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1480606985087" ID="ID_885244508" MODIFIED="1480639465600" TEXT="Layout-Manager">
<node CREATED="1480639469981" ID="ID_983391388" MODIFIED="1480639472833" TEXT="Abstraktionen"/>
<node CREATED="1480639473324" ID="ID_191170582" MODIFIED="1480639510996" TEXT="eval pass"/>
</node>
<node CREATED="1480606973713" ID="ID_163713350" MODIFIED="1480606978899" TEXT="Track-Presenter">
<node CREATED="1480741594101" ID="ID_1017414696" MODIFIED="1480741611300" TEXT="mit doppeltem Display-Kontext verbinden"/>
<node CREATED="1480741545196" FOLDED="true" ID="ID_1657879480" MODIFIED="1488423342662" TEXT="TrackWidget">
<node CREATED="1480741552611" ID="ID_1987203186" MODIFIED="1480741554982" TEXT="erzeugen"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1480741555930" ID="ID_1869427213" MODIFIED="1480741587463">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wie in Kopf <i>und</i>&#160;Rumpf injizieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node CREATED="1480607029673" ID="ID_619470641" MODIFIED="1480607052500" TEXT="Track-Anzeige">
<node CREATED="1480607033512" ID="ID_1815699851" MODIFIED="1480607035268" TEXT="Kopf"/>
<node CREATED="1480607035712" ID="ID_1405339006" MODIFIED="1480607037355" TEXT="Rumpf"/>
</node>
<node CREATED="1480621574221" ID="ID_1217785331" MODIFIED="1480621619049" TEXT="ClipPresenter">
<node CREATED="1480741362389" FOLDED="true" ID="ID_328415412" MODIFIED="1488423342662" TEXT="ClipWidget">
<node CREATED="1480741370347" ID="ID_1275685315" MODIFIED="1480741373623" TEXT="erzeugen"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1480741374451" ID="ID_1270677756" MODIFIED="1480741382099" TEXT="in Display injizieren">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1480741386769" FOLDED="true" ID="ID_1808095573" MODIFIED="1488423342663" TEXT="Kinder">
<node CREATED="1480741389777" ID="ID_1548768233" MODIFIED="1480741396471" TEXT="welche kommen in Frage">
<icon BUILTIN="help"/>
</node>
<node CREATED="1480741397880" ID="ID_299590632" MODIFIED="1480807650331" TEXT="Effekte werden als ClipPresenter implementiert">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1480741425668" ID="ID_826648354" MODIFIED="1487275490115" TEXT="Labels werden als eigenes Tangible MarkerWidget implementiert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...sie verwenden dann ein LabelWidget zur Darstellung
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1480741466519" ID="ID_29080454" MODIFIED="1480741471654" TEXT="das werden zwei Collections">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
<node CREATED="1480607059909" ID="ID_703281238" MODIFIED="1480607061944" TEXT="Control"/>
</node>
</node>
<node CREATED="1479434763643" HGAP="48" ID="ID_1572413636" MODIFIED="1480639177882" TEXT="Clip" VSHIFT="32">
<node CREATED="1479434774138" ID="ID_912862507" MODIFIED="1479601717907" TEXT="Grundlagen">
<node CREATED="1479434780105" ID="ID_239720378" MODIFIED="1487275490131" TEXT="apperances">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ein Clip hat verschiedene Erscheinungsformen im UI
    </p>
    <p>
      
    </p>
    <p>
      Verwende das als Leitgedanke, um das Layout zu entwickeln
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1479434831825" FOLDED="true" ID="ID_975727645" MODIFIED="1488423342663" TEXT="abridged form">
<node CREATED="1479434845992" ID="ID_609815513" MODIFIED="1479434860330" TEXT="media bin, collapsed fork"/>
<node CREATED="1479441548335" ID="ID_150470698" MODIFIED="1479441564712" TEXT="icon + ID label"/>
<node CREATED="1479601640961" ID="ID_191706335" MODIFIED="1479601645229" TEXT="Gtk::Frame"/>
</node>
<node CREATED="1479441503085" FOLDED="true" ID="ID_386209482" MODIFIED="1488423342663" TEXT="compact form">
<node CREATED="1479441530689" ID="ID_1057853337" MODIFIED="1479441539364" TEXT="timeline default"/>
<node CREATED="1479441567268" ID="ID_1413366977" MODIFIED="1479441582078" TEXT="rendered content strip + decorations"/>
<node CREATED="1479601649615" ID="ID_417588278" MODIFIED="1479601662578" TEXT="Gtk::Frame + Gtk::Layout"/>
</node>
<node CREATED="1479441519834" FOLDED="true" ID="ID_1669486876" MODIFIED="1488423342663" TEXT="expanded form">
<node CREATED="1479441586057" ID="ID_64386373" MODIFIED="1479441597593" TEXT="window with embedded sub-clips"/>
<node CREATED="1479601667734" ID="ID_1877644742" MODIFIED="1479601683095" TEXT="dto, with recursive embedded sub-clips"/>
</node>
</node>
<node CREATED="1479442296443" ID="ID_911884433" MODIFIED="1479442304498" TEXT="graceful display degradation">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1479442310537" ID="ID_1849995398" MODIFIED="1479442342361" TEXT="Fall1: wenigstens 1 pixel"/>
<node CREATED="1479442322359" ID="ID_492785140" MODIFIED="1479442337761" TEXT="Fall2: in diesem Pixel ist content"/>
</node>
</node>
<node CREATED="1480797025150" ID="ID_852148145" MODIFIED="1480797028377" TEXT="verwendet f&#xfc;r">
<node CREATED="1480797029581" ID="ID_1257172627" MODIFIED="1480797032121" TEXT="Clips">
<node CREATED="1480797161164" ID="ID_1187665675" MODIFIED="1480797165743" TEXT="abridged"/>
<node CREATED="1480797166355" ID="ID_1788361951" MODIFIED="1480797169462" TEXT="compact"/>
<node CREATED="1480797170331" ID="ID_174323572" MODIFIED="1480797173534" TEXT="expanded"/>
</node>
<node CREATED="1480797032709" ID="ID_597046922" MODIFIED="1480797036144" TEXT="Spuren im Clip"/>
<node CREATED="1480797036804" ID="ID_418077851" MODIFIED="1480797038416" TEXT="Effekte">
<node CREATED="1480797067392" ID="ID_1213128633" MODIFIED="1480797184388" TEXT="compact"/>
<node CREATED="1480797155052" ID="ID_640502487" MODIFIED="1480797188132" TEXT="abridged"/>
</node>
</node>
<node CREATED="1479601690675" ID="ID_542607961" MODIFIED="1479601720226" TEXT="Struktur">
<icon BUILTIN="pencil"/>
<node CREATED="1479601699057" ID="ID_1130364446" MODIFIED="1479601707854" TEXT="Konflikt mit der UI-Bus-Architektur">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1479601736116" ID="ID_891600328" MODIFIED="1479601748486" TEXT="Bus == Rolle des Controler/Model"/>
<node CREATED="1479601749946" ID="ID_887287956" MODIFIED="1479601757981" TEXT="Widget == View-Rolle"/>
</node>
<node CREATED="1479601765896" ID="ID_1632476975" MODIFIED="1479601784408">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      UI-Bus gilt nur f&#252;r <b>globale Belange</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1479601788237" ID="ID_1871715779" MODIFIED="1479601795259" TEXT="wichtige neue Unterscheidung">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1479601796075" ID="ID_1177606400" MODIFIED="1479601807006" TEXT="Global == in der Session aufgezeichnet"/>
<node CREATED="1479601807666" ID="ID_1374495669" MODIFIED="1479601812781" TEXT="vs. lokale UI-Mechanik"/>
<node CREATED="1479601828799" ID="ID_1797519191" MODIFIED="1479601837377" TEXT="Ziel: Subsidiarit&#xe4;t f&#xfc;r lokale Belange"/>
</node>
<node CREATED="1479601884288" HGAP="34" ID="ID_1371609613" MODIFIED="1479602442561" TEXT="L&#xf6;sungs-Idee" VSHIFT="18">
<icon BUILTIN="idea"/>
<node CREATED="1479601889567" ID="ID_1123973572" MODIFIED="1479601899475">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es geht nur um <i>Rollen</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1479601907221" ID="ID_286625648" MODIFIED="1479601914101" TEXT="es ist ein Design-Pattern">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1479601925938" ID="ID_22286545" MODIFIED="1479601944896">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das lokale Element mu&#223; nur als View <i>fungieren</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1479601945839" ID="ID_254470029" MODIFIED="1479601954903" TEXT="Model-View-Presenter">
<icon BUILTIN="forward"/>
</node>
<node CREATED="1479602306718" FOLDED="true" ID="ID_359464754" MODIFIED="1488423342663" TEXT="ClipPresenter">
<node CREATED="1479602311740" ID="ID_963961994" MODIFIED="1479602314256" TEXT="Mittelsmann"/>
<node CREATED="1479602314676" ID="ID_944757170" MODIFIED="1479602318896" TEXT="steht f&#xfc;r den Clip"/>
<node CREATED="1479602319555" ID="ID_1557047586" MODIFIED="1479602327854" TEXT="steuert ein ClipWidget"/>
<node CREATED="1479602338609" FOLDED="true" ID="ID_444253818" MODIFIED="1488423342544" TEXT="ClipWidget">
<node CREATED="1479602341777" ID="ID_1610807382" MODIFIED="1479602381582" TEXT="empf&#xe4;ngt DisplayStrategy"/>
<node CREATED="1479602382195" FOLDED="true" ID="ID_1504834509" MODIFIED="1488423308110">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann sich selbst
    </p>
    <p>
      transformieren
    </p>
  </body>
</html></richcontent>
<node CREATED="1479602412095" ID="ID_832808487" MODIFIED="1479602415154" TEXT="Frame"/>
<node CREATED="1479602415654" ID="ID_1358720" MODIFIED="1479602418642" TEXT="Frame + Layout"/>
<node CREATED="1479602419134" ID="ID_1788706818" MODIFIED="1479602425033" TEXT="sub-Clips im Layout"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1476376943985" HGAP="52" ID="ID_1422206856" MODIFIED="1480639197111" TEXT="Viewer" VSHIFT="10"/>
<node CREATED="1480639186172" HGAP="45" ID="ID_838667304" MODIFIED="1480639193064" TEXT="Mixer" VSHIFT="28"/>
<node CREATED="1476376927660" HGAP="35" ID="ID_688318446" MODIFIED="1479434903774" TEXT="Docks" VSHIFT="12"/>
<node CREATED="1477342616175" HGAP="37" ID="ID_954058801" MODIFIED="1477342623660" TEXT="Workspace" VSHIFT="33">
<node CREATED="1477342624942" ID="ID_56920104" MODIFIED="1477342628449" TEXT="ist-Zustand">
<node CREATED="1477342634181" ID="ID_930340610" MODIFIED="1477342640272" TEXT="ein globaler WindowManager">
<node CREATED="1477342640932" ID="ID_950379507" MODIFIED="1477342666356" TEXT="hat Liste von WorkspaceWindow-s"/>
<node CREATED="1477342694237" ID="ID_1913986184" MODIFIED="1477342711039" TEXT="diese deregistrieren sich beim Schlie&#xdf;en"/>
<node CREATED="1477342711442" ID="ID_500847959" MODIFIED="1477342721373" TEXT="wenn letztes Fenster zu, dann Main-&gt;quit"/>
<node CREATED="1477342816853" ID="ID_205499879" MODIFIED="1487275490251" TEXT="steuert Zusand von Menu&gt;Window&gt;CloseWindow">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...nur <i>enabled</i>&#160;wenn
    </p>
    <p>
      mehr als ein top-level Fenster offen
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1477342868574" ID="ID_679226593" MODIFIED="1477342875681" TEXT="WorkspaceWindow">
<node CREATED="1477342962345" FOLDED="true" ID="ID_1801410798" MODIFIED="1488423342663" TEXT="verbunden mit">
<node CREATED="1477342985710" ID="ID_1944710826" MODIFIED="1477342988103" TEXT="Project"/>
<node CREATED="1477342971448" ID="ID_1574175266" MODIFIED="1477342982842" TEXT="Controller"/>
</node>
<node CREATED="1477343022529" FOLDED="true" ID="ID_1585028268" MODIFIED="1488423342663" TEXT="owns">
<node CREATED="1477343030104" ID="ID_350881879" MODIFIED="1487275490286" TEXT="gtk::UiManager">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      A Gtk::UIManager constructs a user interface (menus and toolbars) from one or more UI definitions,
    </p>
    <p>
      which reference actions from one or more action groups.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1477343041295" FOLDED="true" ID="ID_1874325062" MODIFIED="1488423342545" TEXT="PanelManager">
<node CREATED="1477343244428" ID="ID_513688853" MODIFIED="1477343252255" TEXT="managed die Docks"/>
<node CREATED="1477345531413" ID="ID_1082348568" MODIFIED="1477345540367" TEXT="Liste m&#xf6;glicher Panel-Typen"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1477345852274" FOLDED="true" HGAP="30" ID="ID_1401392249" MODIFIED="1488423308111" TEXT="Kritik" VSHIFT="15">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1477345882862" ID="ID_1342647727" MODIFIED="1477345901848" TEXT="arbeitet zu viel mit Pointern ohe RAII"/>
<node CREATED="1477345906723" ID="ID_906895860" MODIFIED="1477345934939" TEXT="&quot;poor man&apos;s reflection&quot;"/>
<node CREATED="1477345939063" ID="ID_984848221" MODIFIED="1487275490304">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      realisiert Vererbung <i>zu fu&#223;</i>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...anstatt eine auf den konkreten Typ getemplatete Subklasse zu verwenden,
    </p>
    <p>
      wird eine &quot;CreatePanelProc&quot; in einen PanelDescriptor eingewickelt.
    </p>
    <p>
      Letzten Endes wird dieser dann per Match auf die Typ-ID ausgew&#228;hlt.
    </p>
    <p>
      AUA!
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1477347390430" FOLDED="true" HGAP="46" ID="ID_138529757" MODIFIED="1488423308111" TEXT="Panel" VSHIFT="31">
<node CREATED="1477347401045" FOLDED="true" ID="ID_919458304" MODIFIED="1488423308096" TEXT="Timeline">
<node CREATED="1477347410884" ID="ID_507837490" MODIFIED="1477347414199" TEXT="im Umbau"/>
</node>
<node CREATED="1477347405332" ID="ID_1735199230" MODIFIED="1477347406904" TEXT="Asset"/>
<node CREATED="1477347407324" FOLDED="true" ID="ID_1325322495" MODIFIED="1488423308096" TEXT="Viewer">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1477347416659" ID="ID_1220196828" MODIFIED="1477347423723" TEXT="kaputt">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1477347424858" ID="ID_439722309" MODIFIED="1477347445263" TEXT="wird nicht mehr erzeugt">
<icon BUILTIN="clanbomber"/>
</node>
<node CREATED="1477347448199" ID="ID_432416001" MODIFIED="1477347459131" TEXT="Grund: kein Displayer">
<icon BUILTIN="info"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1477616970387" FOLDED="true" HGAP="56" ID="ID_786267715" MODIFIED="1488423308111" TEXT="Problem: Panel clean-up" VSHIFT="40">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1477617009766" ID="ID_205353538" MODIFIED="1487275490327" TEXT="Verdacht: dtor wird nicht aufgerufen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wie komme ich da drauf?
    </p>
    <p>
      Ich wollte untersuchen, ob Gtk::manage( ptr ) korrekt die &#252;bergebenen Objekte aufr&#228;umt.
    </p>
    <p>
      Wie sich nun zeigt, passiert das Aufr&#228;umen im dtor desjenigen Widget, dem das zu managende Objekt als Kind gegeben wurde.
    </p>
    <p>
      Im vorliegenden Fall w&#228;re das der dtor des umschlie&#223;enden ScrolledWindow. Der aber wird <i>offensichtlich nicht aufgerufen,</i>
    </p>
    <p>
      auch nicht im Application-Shutdown!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1477617185582" ID="ID_634277803" MODIFIED="1487275490333" TEXT="PanelManager::createPanels()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....erzeugt wird das hier:
    </p>
    <p>
      dock_.add_item(timelinePanel-&gt;getDockItem(),Gdl::DOCK_BOTTOM);
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
</node>
</node>
<node CREATED="1477343061900" ID="ID_589166101" MODIFIED="1477343063991" TEXT="Statusbar"/>
<node CREATED="1477343071827" ID="ID_369966079" MODIFIED="1487275490342" TEXT="Actions">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Helper to build the menu and for&#160;registering and handling of user action events
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1477344354329" FOLDED="true" ID="ID_1683945104" MODIFIED="1488423342663" TEXT="createUi()">
<node CREATED="1477344605448" ID="ID_960836253" MODIFIED="1477344608195" TEXT="panelManager_.setupDock();"/>
<node CREATED="1477344610087" FOLDED="true" ID="ID_844257690" MODIFIED="1488423342545" TEXT="baut auf">
<node CREATED="1477344616486" ID="ID_466606949" MODIFIED="1477344619001" TEXT="Men&#xfc;bar"/>
<node CREATED="1477344619541" ID="ID_1111690482" MODIFIED="1477344658636" TEXT="dockContainer"/>
<node CREATED="1477344659440" ID="ID_1310811438" MODIFIED="1477344662236" TEXT="statusBar"/>
</node>
</node>
</node>
<node CREATED="1477343115357" ID="ID_213954404" MODIFIED="1477343118368" TEXT="Kritik">
<node CREATED="1477343120660" ID="ID_1940152250" MODIFIED="1487275490362" TEXT="Multiplizit&#xe4;t unklar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es sieht so aus, als w&#228;re es &quot;das&quot; WorkspaceWindow
    </p>
    <p>
      aber es kann davon mehrere geben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1477343152288" ID="ID_1267678981" MODIFIED="1477343164378" TEXT="Vermengung von Fenster und Workspace"/>
</node>
</node>
<node CREATED="1477522265208" HGAP="53" ID="ID_41510242" MODIFIED="1477522301608" TEXT="Neuerungen" VSHIFT="28">
<node CREATED="1477522271822" ID="ID_622217667" MODIFIED="1477522304296" TEXT="PanelExperiment">
<icon BUILTIN="pencil"/>
<node CREATED="1477522330886" ID="ID_1920846276" MODIFIED="1477522338065" TEXT="ScrolledWindow"/>
<node CREATED="1477522340573" FOLDED="true" ID="ID_168076322" MODIFIED="1488423342663" TEXT="Grid">
<node CREATED="1477523590806" ID="ID_908611386" MODIFIED="1477523592705" TEXT="attach()"/>
<node CREATED="1477523593462" ID="ID_1461366621" MODIFIED="1477523597777" TEXT="attach_next_to()"/>
<node CREATED="1477523578208" ID="ID_738525852" MODIFIED="1477523599621" TEXT="add()"/>
</node>
<node CREATED="1477523550484" FOLDED="true" ID="ID_1492266905" MODIFIED="1488423342663" TEXT="Box">
<node CREATED="1477523556667" ID="ID_852178687" MODIFIED="1477523561430" TEXT="horizontal / vertikal"/>
<node CREATED="1477523561930" ID="ID_868197404" MODIFIED="1477523572380" TEXT="pack_start() "/>
<node CREATED="1477523573184" ID="ID_1063091132" MODIFIED="1477523576100" TEXT="pack_end()"/>
</node>
</node>
<node CREATED="1487464476801" ID="ID_1621360845" MODIFIED="1487464479476" TEXT="Umbau">
<node CREATED="1487464480872" ID="ID_830603607" MODIFIED="1487464488651" TEXT="bestehende Strukturen stillegen"/>
<node CREATED="1487464489143" ID="ID_1990719718" MODIFIED="1487464495018" TEXT="noch erhalten als Bezugspunkt"/>
<node CREATED="1487464495494" ID="ID_1238357229" MODIFIED="1487464504929" TEXT="daneben neue Controller-Backbone hochziehen"/>
<node CREATED="1487464505469" ID="ID_707273464" MODIFIED="1487464677579" TEXT="Umbau in neuen top-level">
<arrowlink COLOR="#bc4930" DESTINATION="ID_1868522177" ENDARROW="Default" ENDINCLINATION="221;282;" ID="Arrow_ID_600425644" STARTARROW="None" STARTINCLINATION="1925;196;"/>
</node>
</node>
<node CREATED="1477522287316" ID="ID_1942021610" MODIFIED="1477522290919" TEXT="TimelinePanel"/>
</node>
</node>
</node>
<node CREATED="1488423383765" HGAP="355" ID="ID_1046643705" MODIFIED="1488423403527" TEXT="Verhalten" VSHIFT="26">
<node CREATED="1488423418968" ID="ID_1169811882" MODIFIED="1488423422907" TEXT="Lebenszyklus">
<node CREATED="1488423424111" ID="ID_1090980479" MODIFIED="1488423428307" TEXT="Start"/>
<node CREATED="1488423428942" ID="ID_1847992077" MODIFIED="1488423430034" TEXT="Stop"/>
</node>
<node CREATED="1488423436478" ID="ID_1037695008" MODIFIED="1488423439633" TEXT="Einstieg">
<node CREATED="1488423440629" ID="ID_1295897045" MODIFIED="1488470711327" TEXT="in Timeline">
<linktarget COLOR="#8d8ba5" DESTINATION="ID_1295897045" ENDARROW="Default" ENDINCLINATION="296;-456;" ID="Arrow_ID_150397161" SOURCE="ID_839909208" STARTARROW="None" STARTINCLINATION="1056;0;"/>
<node CREATED="1488423569203" ID="ID_1753722831" MODIFIED="1488423843843" TEXT="TimelinePane anzeigen">
<linktarget COLOR="#a1543e" DESTINATION="ID_1753722831" ENDARROW="Default" ENDINCLINATION="1150;-267;" ID="Arrow_ID_912757548" SOURCE="ID_292894978" STARTARROW="None" STARTINCLINATION="496;-77;"/>
</node>
<node CREATED="1488423579514" ID="ID_1433755621" MODIFIED="1488423586461" TEXT="TimelineWidget erzeugen"/>
<node CREATED="1488470479857" ID="ID_978142233" MODIFIED="1488470483245" TEXT="geschieht...">
<node CREATED="1488470484168" ID="ID_1028709294" MODIFIED="1488470496371" TEXT="explizit auf Anforderung (Window-Men&#xfc;)"/>
<node CREATED="1488470499710" ID="ID_1839340183" MODIFIED="1488470507769" TEXT="automatisch durch Laden der Session"/>
</node>
<node CREATED="1488470528522" HGAP="38" ID="ID_141494228" MODIFIED="1488470537103" TEXT="Ablauf" VSHIFT="11">
<node CREATED="1488470538209" ID="ID_1192459841" MODIFIED="1488470540941" TEXT="Controller"/>
<node CREATED="1488470541432" ID="ID_950213497" MODIFIED="1488470548331" TEXT="holt Model-Repr&#xe4;sentation"/>
<node CREATED="1488470548848" ID="ID_1686896502" MODIFIED="1488470567097" TEXT="wei&#xdf; den Ankerpukt / Zielcontainer"/>
<node CREATED="1488470574388" ID="ID_1331046626" MODIFIED="1488470581879" TEXT="erstellt in diesem ein neues Widget"/>
</node>
</node>
<node CREATED="1488423443916" ID="ID_1213960259" MODIFIED="1488423446288" TEXT="in Assets"/>
<node CREATED="1488423446860" ID="ID_1515465474" MODIFIED="1488423468917" TEXT="Viewer"/>
<node CREATED="1488423478536" ID="ID_1039556823" MODIFIED="1488423481827" TEXT="Projekt"/>
<node CREATED="1488423482303" ID="ID_1337577946" MODIFIED="1488423484627" TEXT="Optionen"/>
</node>
<node CREATED="1488423493014" ID="ID_64529473" MODIFIED="1488423496305" TEXT="Navigation"/>
<node CREATED="1488423514355" ID="ID_917891687" MODIFIED="1488423519006" TEXT="Steuerelemente"/>
</node>
<node CREATED="1448070434915" HGAP="64" ID="ID_257833497" MODIFIED="1450390417230" VSHIFT="7">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      innere
    </p>
    <p>
      Struktur
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#3a8df0" DESTINATION="ID_1618124128" ENDARROW="Default" ENDINCLINATION="-25;-262;" ID="Arrow_ID_539627804" STARTARROW="Default" STARTINCLINATION="-2;35;"/>
<node CREATED="1448070545132" HGAP="31" ID="ID_1410368513" MODIFIED="1453545844231" TEXT="Element" VSHIFT="-7">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="pencil"/>
<node CREATED="1448070580927" ID="ID_467382299" MODIFIED="1488423413030" TEXT="Auspr&#xe4;gungen" VSHIFT="18">
<node CREATED="1448070554963" ID="ID_579919554" MODIFIED="1448070559134" TEXT="Widget"/>
<node CREATED="1448070559466" ID="ID_182430638" MODIFIED="1448070560909" TEXT="Controller"/>
</node>
<node CREATED="1448070583679" HGAP="30" ID="ID_1248152908" MODIFIED="1448658666391" TEXT="Protokoll">
<node CREATED="1448078263344" ID="ID_1136748666" MODIFIED="1448078979113" TEXT="Verhaltensweisen">
<cloud COLOR="#e0d6ba"/>
<icon BUILTIN="info"/>
<node CREATED="1448078268223" FOLDED="true" ID="ID_1027574047" MODIFIED="1491009291493" TEXT="Lebenszyklus">
<node CREATED="1448078450375" ID="ID_459686936" MODIFIED="1487275490428" TEXT="zeugen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: Element registriert sich am UI-Bus
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448078473068" ID="ID_510866826" MODIFIED="1487275490434" TEXT="zerst&#xf6;ren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: Element deregistriert sich am UI-Bus
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1448078308690" FOLDED="true" ID="ID_185716114" MODIFIED="1491009295524" TEXT="Aktionen">
<node CREATED="1448078344541" ID="ID_1781183546" MODIFIED="1487275490443" TEXT="Subjekt einer Aktion">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...ist <i>immer</i>&#160;ein <b>tangible</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448078516094" ID="ID_201691188" MODIFIED="1448078519937" TEXT="Commands"/>
<node CREATED="1448078325679" ID="ID_946047770" MODIFIED="1448078510722" TEXT="tweaks"/>
</node>
<node CREATED="1448078330703" FOLDED="true" ID="ID_1955501343" MODIFIED="1488423342664">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      presentation
    </p>
    <p>
      state
    </p>
  </body>
</html></richcontent>
<node CREATED="1448078381288" FOLDED="true" ID="ID_1570425925" MODIFIED="1488423342545" TEXT="state mark">
<node CREATED="1448078528725" ID="ID_1776152068" MODIFIED="1448078551096">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vom <i>tangible </i>initiiert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448078554385" ID="ID_181795198" MODIFIED="1448078560332" TEXT="wird zur&#xfc;ckgespielt"/>
<node CREATED="1448078678961" FOLDED="true" ID="ID_1905336344" MODIFIED="1488423308111" TEXT="Arten">
<node CREATED="1448078682840" ID="ID_1545251839" MODIFIED="1448078686876" TEXT="zoom"/>
<node CREATED="1448078687176" ID="ID_357365613" MODIFIED="1448078689411" TEXT="scroll"/>
<node CREATED="1448078691727" ID="ID_1874307716" MODIFIED="1448078696019" TEXT="expand"/>
<node CREATED="1448078703294" ID="ID_784991533" MODIFIED="1448078705449" TEXT="Form"/>
</node>
</node>
<node CREATED="1448078385504" FOLDED="true" ID="ID_584410725" MODIFIED="1488423342545" TEXT="feedback">
<node CREATED="1448078569263" ID="ID_906985390" MODIFIED="1448078589473" TEXT="in Fokus bringen"/>
<node CREATED="1448078589997" ID="ID_1779705374" MODIFIED="1448078603503" TEXT="aufblitzen"/>
<node CREATED="1448078604115" ID="ID_666512041" MODIFIED="1448078621580" TEXT="Warn-Nachricht"/>
<node CREATED="1448078622048" ID="ID_1858518915" MODIFIED="1448078624939" TEXT="Fehler"/>
</node>
<node CREATED="1448078416612" FOLDED="true" ID="ID_1392022084" MODIFIED="1488423342545" TEXT="state reset">
<node CREATED="1448078722483" ID="ID_1661890520" MODIFIED="1455421132762" TEXT="auf default!">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1448078731130" ID="ID_1682515267" MODIFIED="1487275490499" TEXT="collapse / expand">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      daf&#252;r gen&#252;gt der normale Reset
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1448078748448" ID="ID_1000763850" MODIFIED="1487275490504" TEXT="Nachrichten l&#xf6;schen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mark &quot;clearMsg&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448078778916" ID="ID_1657108949" MODIFIED="1487275490510" TEXT="Fehler l&#xf6;schen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mark &quot;clearErr&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448078798369" ID="ID_819750758" MODIFIED="1487275490516" TEXT="komplett-Reset">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mark &quot;reset&quot;
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1448078425594" FOLDED="true" ID="ID_686619553" MODIFIED="1491009288318" TEXT="Mutation">
<icon BUILTIN="pencil"/>
<node CREATED="1448078811895" ID="ID_1266803050" MODIFIED="1448078833749">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Nachricht an <i>irgend ein</i>&#160;Wurzel-Element
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448078835044" ID="ID_358413966" MODIFIED="1448078844783" TEXT="bezieht daraufhin einen Diff"/>
<node CREATED="1448078845419" ID="ID_189415188" MODIFIED="1448078851438" TEXT="wendet diesen rekursiv an"/>
<node CREATED="1448078856722" ID="ID_1770248185" MODIFIED="1454975518746" TEXT="Hinweis: kann Kinder zeugen und zerst&#xf6;ren"/>
</node>
</node>
</node>
<node CREATED="1448658633478" HGAP="99" ID="ID_314439240" MODIFIED="1448658665728" TEXT="Design" VSHIFT="13">
<node CREATED="1448658692023" ID="ID_339186676" MODIFIED="1487275490536">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      generisch
    </p>
    <p>
      sinnvoll?
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      was haben alle UI-Elemente wirklich gemeinsam?
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1448658692023" FOLDED="true" ID="ID_888978058" MODIFIED="1488423342664" TEXT="generische Commands">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die Frage ist, wie generisch ist eigentlich ein Command-Aufruf selber?
    </p>
    <p>
      Macht es daher Sinn, ein generisches API allgemein sichtbar zu machen,
    </p>
    <p>
      oder handelt es sich nur um ein Implementierungsdetail der UI-Bus-Anbindung?
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1448659456882" ID="ID_946761955" MODIFIED="1448659469420" TEXT="Notwendig: Command-Name + Varargs"/>
<node CREATED="1451093919426" ID="ID_56876272" MODIFIED="1451093932614" TEXT="generisch impliziert auch zentrale Definition">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1451093994135" ID="ID_1002329025" MODIFIED="1451094104862">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wird sinnvoll im Rahmen von <font color="#8e11a1">InteractionControl</font>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#b567c4" DESTINATION="ID_1009169288" ENDARROW="Default" ENDINCLINATION="254;0;" ID="Arrow_ID_200182911" STARTARROW="None" STARTINCLINATION="162;0;"/>
</node>
</node>
<node CREATED="1448659723879" FOLDED="true" ID="ID_197701429" MODIFIED="1488423342664" TEXT="UI-Grundverhalten">
<node CREATED="1448659735557" ID="ID_1553949303" MODIFIED="1448659741744" TEXT="Falten / Expandieren"/>
<node CREATED="1448659747476" ID="ID_1549415142" MODIFIED="1448659756582" TEXT="Grad der Explizitheit"/>
<node CREATED="1448659757378" ID="ID_1164836827" MODIFIED="1448659767668" TEXT="in Sicht bringen"/>
<node CREATED="1448659780743" ID="ID_1787132400" MODIFIED="1448659785106" TEXT="Fokus-Management"/>
<node CREATED="1448659792782" ID="ID_1547056651" MODIFIED="1448659801312" TEXT="r&#xe4;umliche Navigation"/>
<node CREATED="1448659824337" ID="ID_1734858009" MODIFIED="1448659829865" TEXT="Markierungen"/>
</node>
<node CREATED="1455236472002" FOLDED="true" HGAP="49" ID="ID_952544600" MODIFIED="1488423342664" TEXT="generische Eigenschaften" VSHIFT="15">
<icon BUILTIN="button_cancel"/>
<node CREATED="1455236491432" ID="ID_1268644179" MODIFIED="1455236569787" TEXT="Idee">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1455236501719" HGAP="35" ID="ID_540888754" MODIFIED="1455236634083" TEXT="man k&#xf6;nnte Eigenschaften konfigurieren" VSHIFT="2"/>
<node CREATED="1455236512373" FOLDED="true" ID="ID_211355731" MODIFIED="1488423342545" TEXT="jede Eigenschaft h&#xe4;tte">
<node CREATED="1455236519677" ID="ID_1698754255" MODIFIED="1455236521384" TEXT="Namen"/>
<node CREATED="1455236522020" ID="ID_404018073" MODIFIED="1455236526855" TEXT="slot"/>
<node CREATED="1455236527443" ID="ID_45035009" MODIFIED="1455236537118" TEXT="passende &#xbb;state mark&#xab;"/>
<node CREATED="1455236537802" ID="ID_1419063243" MODIFIED="1455236554532" TEXT="handler-funktor"/>
<node CREATED="1455236554927" ID="ID_772542993" MODIFIED="1455236563746" TEXT="installiert sich in generischen handler"/>
</node>
<node CREATED="1455236573173" FOLDED="true" HGAP="72" ID="ID_466911779" MODIFIED="1488423342545" TEXT="Bewertung" VSHIFT="16">
<icon BUILTIN="yes"/>
<node CREATED="1455236781242" ID="ID_674694140" MODIFIED="1455236787861" TEXT="k&#xf6;nnte interessant sein f&#xfc;r Diff"/>
<node CREATED="1455236788424" ID="ID_1507269502" MODIFIED="1455236848778" TEXT="aber gegen das Grundkonzept">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ich wollte explizit kein generisch-introspektives UI,
    </p>
    <p>
      weil das die Tendenz hat, sich zu einem Framework auszuwachsen.
    </p>
    <p>
      F&#252;r die UI-Programmierung mu&#223; man Spaghetticode akzeptieren.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455236851592" ID="ID_1599276883" MODIFIED="1455236859119" TEXT="YAGNI">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1455236869405" ID="ID_1141383110" MODIFIED="1455236874756" TEXT="sp&#xe4;ter">
<icon BUILTIN="hourglass"/>
</node>
</node>
</node>
</node>
<node CREATED="1448658696638" ID="ID_1235910089" MODIFIED="1448658704649" TEXT="slots">
<node CREATED="1451177440022" FOLDED="true" ID="ID_1603816152" MODIFIED="1488423342664" TEXT="expand">
<icon BUILTIN="button_ok"/>
<node CREATED="1451177449405" ID="ID_215708646" MODIFIED="1451177458839" TEXT="zun&#xe4;chst einfach auf/zu"/>
<node CREATED="1451177459667" ID="ID_1151970636" MODIFIED="1487275490604" TEXT="wird m&#xf6;glicherweise eine Enum">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint, eine ENUM von verschiedenen Graden der Aufgeklappt-heit
    </p>
    <p>
      Dann mu&#223;te das allerdigns jeweils f&#252;r alle Elemente sinnvoll sein
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1451177534345" FOLDED="true" ID="ID_524388510" MODIFIED="1488423342545" TEXT="impl">
<node CREATED="1451177542256" ID="ID_980888532" MODIFIED="1487275490614" TEXT="ruft einen konkreten hook doExpand() auf">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und der mu&#223; vom konkreten Widget implementiert werden
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1451177573108" ID="ID_1154342108" MODIFIED="1451177583935" TEXT="wenn dieser true zur&#xfc;ckgibt, ist der Zustand persistent"/>
<node CREATED="1451177584435" ID="ID_1792154974" MODIFIED="1451177606906">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      dann wird eine <i>state mark</i>&#160;ausgesendet
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1451177442861" FOLDED="true" ID="ID_1257047796" MODIFIED="1488423342664" TEXT="reveal">
<icon BUILTIN="smily_bad"/>
<node CREATED="1451177636388" ID="ID_507693817" MODIFIED="1451177655664" TEXT="&quot;bringe dies Element in Sicht&quot;">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1451177626773" ID="ID_612921051" MODIFIED="1451177634927" TEXT="ist das eine gute Idee">
<icon BUILTIN="help"/>
</node>
<node CREATED="1451177659369" ID="ID_928484705" MODIFIED="1451177677957">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      need to <i>bubble up</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1451177719577" ID="ID_1175980404" MODIFIED="1451177726636" TEXT="wie finde ich den Vater..."/>
<node CREATED="1455236368345" FOLDED="true" ID="ID_1490676072" MODIFIED="1488423342545" TEXT="besser">
<icon BUILTIN="yes"/>
<node CREATED="1455236382183" ID="ID_983670310" MODIFIED="1455236392849" TEXT="Einstiegspunkt: revealYourself"/>
<node CREATED="1455236393861" ID="ID_1638665625" MODIFIED="1455236410123">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      support ist <i>optional</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455236418290" ID="ID_1765760054" MODIFIED="1455236435380" TEXT="dieses ruft parent.reveal(myID)"/>
</node>
</node>
</node>
<node CREATED="1455421282030" HGAP="54" ID="ID_1448430954" MODIFIED="1488423306897" TEXT="state mark" VSHIFT="12">
<icon BUILTIN="button_ok"/>
<node CREATED="1455421310603" ID="ID_1677758546" MODIFIED="1455421335388" TEXT="slots und markXX sind aufgedoppelt"/>
<node CREATED="1455421372274" FOLDED="true" ID="ID_956411027" MODIFIED="1488423342665" TEXT="Implementierung mu&#xdf; emittieren">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1455421336808" ID="ID_520325590" MODIFIED="1455421370759" TEXT="wenn eine &#xc4;nderung relevant f&#xfc;r Persistenz"/>
<node CREATED="1455421392032" ID="ID_969248847" MODIFIED="1455421405946" TEXT="die doXXX() - Funktion gibt true zur&#xfc;ck"/>
<node CREATED="1455421407646" ID="ID_480078096" MODIFIED="1455421421439" TEXT="dann erledigt die slot / mark-Funktion das"/>
</node>
<node CREATED="1455421435698" FOLDED="true" ID="ID_799737882" MODIFIED="1488423342665" TEXT="PresentationStateManager sammelt einfach">
<icon BUILTIN="button_ok"/>
<node CREATED="1455421453040" ID="ID_1195489674" MODIFIED="1455421460035" TEXT="pro Element (=ID)"/>
<node CREATED="1455421460671" ID="ID_1160333722" MODIFIED="1455421483559" TEXT="pro &quot;property&quot; == ID-sym der state mark - Nachricht"/>
</node>
<node CREATED="1455421490643" FOLDED="true" ID="ID_256291546" MODIFIED="1488423342665" TEXT="L&#xf6;sch / Reset-Nachrichten">
<icon BUILTIN="idea"/>
<node CREATED="1455421507505" ID="ID_1382752422" MODIFIED="1455421676233" TEXT="sind auszusenden, wenn tats&#xe4;chlich gel&#xf6;scht wurde">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1455421520087" ID="ID_1219279890" MODIFIED="1455421531914" TEXT="werden vom PresentationStateManager speziell behandelt"/>
</node>
<node CREATED="1455421537717" FOLDED="true" ID="ID_562890111" MODIFIED="1488423342665" TEXT="k&#xf6;nnen per Broadcast verbreitet werden">
<node CREATED="1455421549347" ID="ID_744479545" MODIFIED="1455421556318" TEXT="separates UI-Bus API"/>
<node CREATED="1455421557026" ID="ID_655863988" MODIFIED="1455421654735">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nach Broadcast von &quot;reset&quot;
    </p>
    <p>
      sollte logischerweise der PresentationStateManager leer sein
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ist er aber nicht notwendig,
    </p>
    <p>
      denn er kann Zustand von nicht mehr existierenden Elementen aufgezeichnet haben.
    </p>
    <p>
      Nur Elemente, die im Moment angeschlossen sind, bekommen die &quot;reset&quot;-Nachricht mit;
    </p>
    <p>
      sofern sie tats&#228;chlich abweichenden Zustand haben, sollten sie sich resetten
    </p>
    <p>
      und eine state mark &quot;reset&quot; zur&#252;ckschicken...
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1455422030995" FOLDED="true" HGAP="35" ID="ID_1455265273" MODIFIED="1491009391795" TEXT="mutation" VSHIFT="7">
<icon BUILTIN="pencil"/>
<node CREATED="1455666294927" FOLDED="true" ID="ID_373207685" MODIFIED="1488423342665" TEXT="Ausgangspunkt">
<node CREATED="1455666301630" ID="ID_933452284" MODIFIED="1455666310209" TEXT="Element-Protokoll steht"/>
<node CREATED="1455666310701" ID="ID_571166725" MODIFIED="1455666314472" TEXT="Bus-Semantik steht"/>
<node CREATED="1455666315061" ID="ID_1650938256" MODIFIED="1455666328663" TEXT="Konzept f&#xfc;r Tree-Mutator steht"/>
<node CREATED="1455666330652" ID="ID_654052510" MODIFIED="1455668788278" TEXT="Tree-Diff-Applikator ist implementiert"/>
<node CREATED="1455666351784" ID="ID_1889806972" MODIFIED="1455666364225" TEXT="Entscheidung f&#xfc;r 2-Phasiges Diff-Update ist gefallen"/>
</node>
<node CREATED="1455668350028" FOLDED="true" HGAP="43" ID="ID_1853535891" MODIFIED="1488423342665" TEXT="Ablauf" VSHIFT="5">
<node CREATED="1455668396398" ID="ID_1627727409" MODIFIED="1455668828239" TEXT="Diff-Nachricht kommt via UI-Bus">
<icon BUILTIN="full-1"/>
</node>
<node CREATED="1455668411867" ID="ID_1932196729" MODIFIED="1455668830612" TEXT="UI-Element baut Tree-Mutator &quot;auf sich selbst&quot;">
<icon BUILTIN="full-2"/>
</node>
<node CREATED="1455668689495" ID="ID_1046261411" MODIFIED="1455668833289" TEXT="UI-Element setzt Tree-Diff-Applikator auf diesen an">
<icon BUILTIN="full-3"/>
</node>
</node>
<node CREATED="1455668897947" FOLDED="true" HGAP="211" ID="ID_1897861223" MODIFIED="1491009320473" TEXT="Konsequenzen" VSHIFT="23">
<node CREATED="1455668923175" ID="ID_1183550957" MODIFIED="1472219338326" TEXT="brauche passendes UI-Bus API">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....so harmlos hat alles angefangen
    </p>
    <p>
      
    </p>
    <p>
      Denn das hei&#223;t, ich mu&#223; konkret ausarbeiten,
    </p>
    <p>
      <i>wie</i>&#160;man einen Diff gegen eine opaque Implementierungs-Datenstruktur aufspielt.
    </p>
    <p>
      Und ich mu&#223; das in einem Test zumindest emulieren k&#246;nnen!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
<node CREATED="1455668935142" FOLDED="true" ID="ID_1274632216" MODIFIED="1488423342546">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mu&#223; DiffApplicationStrategy
    </p>
    <p>
      <i>noch einmal</i>&#160;implementieren
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das mag &#252;berraschen --
    </p>
    <p>
      ist aber im Sinne des Erfinders
    </p>
    <ul>
      <li>
        DiffApplicationStrategy war von Anfang an als technisches Binding konzipiert
      </li>
      <li>
        es ist besser, die gleiche Semantik der Sprache X-mal herunterzucoden
      </li>
      <li>
        cleverer Code-re-Use zahlt sich i.d.R. nicht aus
      </li>
    </ul>
  </body>
</html></richcontent>
<node CREATED="1455669272760" FOLDED="true" ID="ID_837691598" MODIFIED="1488423308111" TEXT="Call-Stack ist der Prozessor-Stack (Rekursion)">
<icon BUILTIN="button_cancel"/>
<node CREATED="1455899000582" ID="ID_1404453203" MODIFIED="1455899086452" TEXT="sorry, geht nicht">
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1455899005885" FOLDED="true" ID="ID_1920318464" MODIFIED="1488423308096" TEXT="denn sonst mu&#xdf; ich">
<icon BUILTIN="idea"/>
<node CREATED="1455899015220" ID="ID_1476900463" MODIFIED="1455899023991" TEXT="entweder die Anwendung komplett neu schreiben"/>
<node CREATED="1455899024459" ID="ID_1562031965" MODIFIED="1455899033805" TEXT="oder den DiffApplicator umbauen"/>
<node CREATED="1455899034129" ID="ID_1505454112" MODIFIED="1455899053610" TEXT="und das reicht bis in die Diff-Verben..."/>
<node CREATED="1455899054575" ID="ID_1973501142" MODIFIED="1455899068441" TEXT="die dann rekursiv vom Diff-Typ abh&#xe4;ngig werden"/>
<node CREATED="1455899068941" ID="ID_604248716" MODIFIED="1455899072752" TEXT="NEE DANKE"/>
</node>
</node>
<node CREATED="1455669266289" ID="ID_476883926" MODIFIED="1455899233976" TEXT="mu&#xdf; ebenfalls expliziten Stack verwenden">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1455899121277" FOLDED="true" ID="ID_1897194874" MODIFIED="1488423308111" TEXT="eigentlich ist das ganze Copy-n-Paste">
<icon BUILTIN="button_ok"/>
<node CREATED="1455899176175" ID="ID_589009361" MODIFIED="1455899183929" TEXT="gef&#xfc;hlsm&#xe4;&#xdf;ig: hier kein Problem"/>
<node CREATED="1455899184397" ID="ID_933775328" MODIFIED="1455899191320" TEXT="denn die Implementierung ist gekoppelt"/>
<node CREATED="1455899191836" ID="ID_1325546784" MODIFIED="1455899200983" TEXT="mu&#xdf; gekoppelt sein, wg. Effizienz"/>
<node CREATED="1455899201435" ID="ID_1353317149" MODIFIED="1455899210798" TEXT="eine Abstraktion hat hier keinen Mehrwert"/>
<node CREATED="1455899211338" ID="ID_1152596163" MODIFIED="1455899216645" TEXT="und ist &#xfc;berall sonst eine B&#xfc;rde"/>
</node>
<node CREATED="1472829783619" HGAP="28" ID="ID_1015946734" MODIFIED="1472829805342" TEXT="am Ende ist das die sch&#xf6;nere Implementierung geworden" VSHIFT="5">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1455669004941" FOLDED="true" ID="ID_853385575" MODIFIED="1488423342546">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      dies setzt volle Implementierung
    </p>
    <p>
      des Tree-Mutators voraus
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      der schwierigste Teil, das Mutieren von Attributen,
    </p>
    <p>
      ist jedoch schon prototypisch implementiert
    </p>
  </body>
</html></richcontent>
<node CREATED="1455899288247" FOLDED="true" ID="ID_635341866" MODIFIED="1488423308111" TEXT="Tree-Mutator hat echte (Assignment)-Mutation">
<icon BUILTIN="info"/>
<node CREATED="1455899303981" ID="ID_1584621992" MODIFIED="1455899333910" TEXT="pa&#xdf;t nicht auf die Semantik vom Diff">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1455899315092" ID="ID_1911158046" MODIFIED="1455899330748" TEXT="dann eben passend machen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1455899321418" ID="ID_802682081" MODIFIED="1455899337153" TEXT="Diff bekommt ein SET verb">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1455669313986" FOLDED="true" ID="ID_1730130372" MODIFIED="1488423308111" TEXT="brauche generiischen Rahmen">
<node CREATED="1455669322561" ID="ID_1968219875" MODIFIED="1455669325957" TEXT="f&#xfc;r skip"/>
<node CREATED="1455669326400" ID="ID_646237490" MODIFIED="1455669330996" TEXT="f&#xfc;r ins / del"/>
<node CREATED="1455669331448" ID="ID_1852578403" MODIFIED="1455669333036" TEXT="f&#xfc;r find"/>
</node>
</node>
<node COLOR="#ad014c" CREATED="1455669200945" FOLDED="true" HGAP="85" ID="ID_880765624" MODIFIED="1488423342546" TEXT="Design-Fragen" VSHIFT="-14">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_ok"/>
<node COLOR="#4e1527" CREATED="1458408700205" FOLDED="true" ID="ID_685778920" MODIFIED="1488423308111" TEXT="Grundansatz">
<font NAME="SansSerif" SIZE="15"/>
<icon BUILTIN="yes"/>
<node CREATED="1458408708004" ID="ID_239878029" MODIFIED="1458408717247" TEXT="Abstraktion / Adapter"/>
<node CREATED="1458408717739" ID="ID_579166238" MODIFIED="1458408726086" TEXT="(virtuelles) Interface"/>
<node CREATED="1458408730841" ID="ID_356597537" MODIFIED="1458408739164" TEXT="konkretes Binding"/>
<node CREATED="1458408742408" FOLDED="true" ID="ID_1733136177" MODIFIED="1488423308096" TEXT="Binding-Technik">
<node CREATED="1458408757478" ID="ID_1718466382" MODIFIED="1458408786501" TEXT="Mutator wird in jeweiliger Impl erzeugt"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#221c4f" CREATED="1458408790457" FOLDED="true" ID="ID_1892865523" MODIFIED="1488423308077" STYLE="fork" TEXT="Polymorphie vs. Funktoren">
<icon BUILTIN="button_ok"/>
<node CREATED="1458408878574" FOLDED="true" ID="ID_1190262572" MODIFIED="1488423308055" TEXT="Abw&#xe4;gung">
<icon BUILTIN="info"/>
<node CREATED="1458408895756" FOLDED="true" ID="ID_1422093195" MODIFIED="1488423306944" TEXT="polymorph">
<node CREATED="1458408909594" ID="ID_447360746" MODIFIED="1458409828754">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Mutator verwendet einen <i>Binder</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458408965770" ID="ID_1892856741" MODIFIED="1458409828754" TEXT="jeweiliger Client implementiert den Binder"/>
<node CREATED="1458409675740" ID="ID_1319191337" MODIFIED="1458409828754" TEXT="Implementierung direkt durch Typsystem geleitet">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1458408946813" ID="ID_86057383" MODIFIED="1458409828754" TEXT="Indirektion, virtuelle Funktionen">
<icon BUILTIN="clanbomber"/>
</node>
<node CREATED="1458409155585" ID="ID_1985086830" MODIFIED="1458409828754" TEXT="Client mu&#xdf; explizit instantiieren und verbinden">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1458409202946" ID="ID_162172192" MODIFIED="1458409828754" TEXT="f&#xfc;r jede Anwendungssituation eine neue Binder-Klasse"/>
<node CREATED="1458409270353" ID="ID_114384372" MODIFIED="1458409828754" TEXT="plus f&#xfc;r jedes Anwendungs-Grundmuster eine Mutator-Implementierung"/>
</node>
<node CREATED="1458408904802" FOLDED="true" ID="ID_1569962545" MODIFIED="1488423306945" TEXT="generisch">
<node CREATED="1458409289263" ID="ID_1520752432" MODIFIED="1458409828754" TEXT="konkreter Mutator wird aus Bausteinen zusammengestellt"/>
<node CREATED="1458409313524" ID="ID_90057804" MODIFIED="1458409828754" TEXT="diese Bausteine verwenden Funktoren und Lambdas"/>
<node CREATED="1458409343504" ID="ID_1243373098" MODIFIED="1458409828754" TEXT="Mutator-Instantiierung und Lamdas im gleichen Scope"/>
<node CREATED="1458409357886" ID="ID_102579030" MODIFIED="1458409828754" TEXT="Bindung implizit, Inlining f&#xfc;r Lambdas"/>
<node CREATED="1458409636857" ID="ID_410838714" MODIFIED="1458409828754" TEXT="Typ/Signaturfehler werden (erst) im generischen Code bemerkt">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1458409384661" ID="ID_599058435" MODIFIED="1458409828754" TEXT="f&#xfc;r jede Anwendungssituation eine neue Mutator-Implementierung"/>
<node CREATED="1458409554491" ID="ID_1516498705" MODIFIED="1458409828754" TEXT="aber auch Builder-Zwischenstufen m&#xfc;sen als Funktionen instantiiert werden">
<icon BUILTIN="clanbomber"/>
</node>
</node>
</node>
<node CREATED="1458408881773" FOLDED="true" ID="ID_649871974" MODIFIED="1488423308055" TEXT="Entscheidung">
<icon BUILTIN="yes"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#010101" CREATED="1458409428796" ID="ID_1590872717" MODIFIED="1458409828754" TEXT="generisch">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1458409480646" ID="ID_239846348" MODIFIED="1458409828754" TEXT="weniger Indirektion"/>
<node CREATED="1458409499547" ID="ID_734782069" MODIFIED="1458409828755" TEXT="geringf&#xfc;gig weniger Code-bloat"/>
<node CREATED="1458409506778" ID="ID_1696279423" MODIFIED="1458409828755" TEXT="bessere Chancen auf Inlining"/>
<node CREATED="1458409586672" FOLDED="true" ID="ID_902822558" MODIFIED="1488423306945" TEXT="Client-Code">
<node CREATED="1458409608924" ID="ID_1425639317" MODIFIED="1458409828755" TEXT="kompakter"/>
<node CREATED="1458409611484" ID="ID_1386932549" MODIFIED="1458409828755" TEXT="expliziter"/>
<node CREATED="1458409616980" ID="ID_1067167042" MODIFIED="1458409828755" TEXT="weniger technisch"/>
<node CREATED="1458409622475" ID="ID_858066487" MODIFIED="1458409828755" TEXT="lesbarer"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1455669221255" FOLDED="true" ID="ID_1712552545" MODIFIED="1488423308112" TEXT="Repr&#xe4;sentation der &quot;aktuellen Position&quot;">
<node CREATED="1472829837156" ID="ID_1214532087" MODIFIED="1472829843934" TEXT="implizit lassen"/>
<node CREATED="1472829844387" ID="ID_1395134655" MODIFIED="1472829852134" TEXT="auf Implementierungs-Ebene verlagern"/>
<node CREATED="1472829853242" ID="ID_562014580" MODIFIED="1472829913272" TEXT="in jedem Layer / f&#xfc;r jeden Typ gesondert">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_562014580" ENDARROW="Default" ENDINCLINATION="451;-8;" ID="Arrow_ID_543974195" SOURCE="ID_631826706" STARTARROW="None" STARTINCLINATION="322;-32;"/>
</node>
</node>
<node CREATED="1455669345167" ID="ID_310849010" MODIFIED="1455669353721" TEXT="generischen Rahmen f&#xfc;r Mut-Operationen"/>
<node CREATED="1455669238213" FOLDED="true" ID="ID_731780565" MODIFIED="1488423308112" TEXT="Einleiten der Rekursion">
<node CREATED="1455842313629" ID="ID_179960248" MODIFIED="1455842321264" TEXT="ich h&#xe4;tte es gern echt-rekursiv"/>
<node CREATED="1455842321828" ID="ID_1622068738" MODIFIED="1455842336974" TEXT="widerspricht aber unserem DiffApplicator"/>
</node>
<node CREATED="1455833678448" FOLDED="true" HGAP="47" ID="ID_1439118587" MODIFIED="1488423308112" TEXT="Probleme" VSHIFT="16">
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="clanbomber"/>
<node CREATED="1455833736586" FOLDED="true" ID="ID_1233162987" MODIFIED="1488423308096" TEXT="Analyse: Feld vs Attribut">
<icon BUILTIN="info"/>
<node CREATED="1455834007278" FOLDED="true" ID="ID_196347476" MODIFIED="1488423308077" TEXT="Feld">
<node CREATED="1455834060110" ID="ID_1872959165" MODIFIED="1455834067177" TEXT="ist da per Struktur"/>
<node CREATED="1455834067837" ID="ID_944456076" MODIFIED="1455834075400" TEXT="hat immer einen Wert"/>
</node>
<node CREATED="1455834010264" FOLDED="true" ID="ID_1604005834" MODIFIED="1488423308077" TEXT="Attribut">
<node CREATED="1455834109112" ID="ID_879864381" MODIFIED="1455834119178" TEXT="wird per Key angesprochen"/>
<node CREATED="1455834121926" ID="ID_504132661" MODIFIED="1455834189337" TEXT="kann abwesend sein"/>
</node>
<node CREATED="1455834013389" FOLDED="true" ID="ID_133168935" MODIFIED="1488423308078" TEXT="Widerspr&#xfc;che">
<node CREATED="1455834227528" ID="ID_1606302361" MODIFIED="1455834231179" TEXT="Feld hinzuf&#xfc;gen"/>
<node CREATED="1455834231647" ID="ID_1275813367" MODIFIED="1455834236914" TEXT="Feld l&#xf6;schen"/>
<node CREATED="1455834237999" ID="ID_254859574" MODIFIED="1455834251313" TEXT="Feld suchen"/>
<node CREATED="1455834400273" ID="ID_54362263" MODIFIED="1455834403925" TEXT="Zuweisung"/>
<node CREATED="1455834252068" ID="ID_1918373755" MODIFIED="1455834285644" TEXT="Ordnung"/>
</node>
</node>
<node CREATED="1455833774965" FOLDED="true" ID="ID_677756898" MODIFIED="1488423308096" TEXT="&#xc4;ndern wider die Natur">
<node CREATED="1455898722195" ID="ID_1299759861" MODIFIED="1455898728644" TEXT="Problem delegieren..."/>
<node CREATED="1455898713124" ID="ID_1862448261" MODIFIED="1455898720143" TEXT="ignorieren bzw. scheitern lassen"/>
</node>
<node CREATED="1455833858794" FOLDED="true" ID="ID_483179827" MODIFIED="1488423308096" TEXT="zus&#xe4;tzliche Ordnungsstruktur">
<node CREATED="1455898689479" ID="ID_1223705901" MODIFIED="1455898696898" TEXT="Attribut: ignorieren"/>
<node CREATED="1455898698670" ID="ID_1532643598" MODIFIED="1455898705001" TEXT="Kind: delegieren"/>
</node>
<node CREATED="1455842653928" FOLDED="true" ID="ID_1996966445" MODIFIED="1488423308096">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Diff kennt keine <i>Zuweisung</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1455842674973" ID="ID_1910702757" MODIFIED="1455842682008" TEXT="L&#xf6;schen + Neueinf&#xfc;gen"/>
<node CREATED="1455842682524" ID="ID_1597781391" MODIFIED="1455842710444" TEXT="unn&#xf6;tiger Aufwand (Allokation)"/>
<node CREATED="1455842711008" ID="ID_1470911011" MODIFIED="1455842721083" TEXT="f&#xfc;hrt zu &quot;Flackern&quot; im UI"/>
<node CREATED="1455843001833" FOLDED="true" HGAP="46" ID="ID_617738816" MODIFIED="1488423308078" TEXT="L&#xf6;sungen" VSHIFT="16">
<icon BUILTIN="idea"/>
<node CREATED="1455843011048" FOLDED="true" ID="ID_1280199494" MODIFIED="1488423308056" TEXT="Spezialvereinbarung">
<icon BUILTIN="button_cancel"/>
<node CREATED="1455843048699" ID="ID_1949875245" MODIFIED="1455843054766" TEXT="Einf&#xfc;gen des gleichen Attributes"/>
<node CREATED="1455843055122" ID="ID_880285017" MODIFIED="1455843061285" TEXT="geht nur unmittelbar danach"/>
<node CREATED="1455843061633" ID="ID_1466937234" MODIFIED="1455843065597" TEXT="geht nur mit Attributen"/>
</node>
<node CREATED="1455843068672" FOLDED="true" ID="ID_695348733" MODIFIED="1488423308056" TEXT="neues Verb">
<icon BUILTIN="button_ok"/>
<node CREATED="1455843074288" ID="ID_251586851" MODIFIED="1455843077987" TEXT="SET"/>
<node CREATED="1455843120961" ID="ID_1340712241" MODIFIED="1455843131556" TEXT="setzt Element-Identit&#xe4;t voraus"/>
<node CREATED="1455843134111" ID="ID_1830155209" MODIFIED="1455843174534" TEXT="funkioniert (theoretisch) auch bei Kindern"/>
<node CREATED="1455843262862" FOLDED="true" ID="ID_779223103" MODIFIED="1488423306951" TEXT="Frage: Auto-PICK">
<icon BUILTIN="help"/>
<node CREATED="1455843317119" ID="ID_755404987" MODIFIED="1455895650565" TEXT="Ja: aber was dann mit Umordnungen">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1455843336405" FOLDED="true" ID="ID_1210482014" MODIFIED="1488423306951">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Nein
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1455895664140" ID="ID_639070488" MODIFIED="1455895669295">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aber was dann wenn out-of-order
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1455895670603" FOLDED="true" ID="ID_1629566976" MODIFIED="1488423306951" TEXT="out-of-order">
<icon BUILTIN="yes"/>
<node CREATED="1455895687377" ID="ID_742953981" MODIFIED="1455895691763" TEXT="wird unterst&#xfc;tzt"/>
<node CREATED="1455895692304" ID="ID_1361195241" MODIFIED="1455895769838" TEXT="denn: analog zu mut-emu">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1455895701215" ID="ID_1720750220" MODIFIED="1455895710033" TEXT="erm&#xf6;glicht Postfix-Order"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1455913713933" FOLDED="true" ID="ID_155948375" MODIFIED="1488423308096" TEXT="getypte Kinder">
<icon BUILTIN="help"/>
<node CREATED="1455913726963" FOLDED="true" ID="ID_514338534" MODIFIED="1488423308078" TEXT="kommt es vor">
<icon BUILTIN="help"/>
<node CREATED="1455926476893" ID="ID_1938999393" MODIFIED="1455926499782" TEXT="ja"/>
<node CREATED="1455926509185" ID="ID_784567614" MODIFIED="1455926521747" TEXT="in der Filter-Pipe"/>
<node CREATED="1455926675370" FOLDED="true" ID="ID_572508223" MODIFIED="1488423308056" TEXT="&#xbb;Zwei-Listen-Modell&#xab;">
<node CREATED="1455926709229" ID="ID_272389445" MODIFIED="1455926710513" TEXT="f&#xfc;r generischen Diff"/>
<node CREATED="1455926710981" ID="ID_1449723679" MODIFIED="1455926717032" TEXT="m&#xf6;gliche Vereinheitlichung"/>
<node CREATED="1455926717452" ID="ID_1884046427" MODIFIED="1455926728191" TEXT="generisch, nicht polymorph"/>
</node>
</node>
<node CREATED="1455913731723" FOLDED="true" ID="ID_1269232747" MODIFIED="1488423308078" TEXT="Problem umgehen">
<icon BUILTIN="help"/>
<node CREATED="1455926525990" ID="ID_1102781575" MODIFIED="1455926538168" TEXT="w&#xe4;re m&#xf6;glich"/>
<node CREATED="1455926539204" ID="ID_650449853" MODIFIED="1455926543144" TEXT="Polymorphismus"/>
</node>
<node CREATED="1455913800081" FOLDED="true" ID="ID_552808081" MODIFIED="1488423308078" TEXT="Typ-Info transportieren">
<icon BUILTIN="help"/>
<node CREATED="1455926589909" ID="ID_1341677743" MODIFIED="1455926610247" TEXT="Form der ID"/>
<node CREATED="1455926610931" ID="ID_1390833573" MODIFIED="1455926615310" TEXT="Pr&#xe4;fix in der ID"/>
<node CREATED="1455926617866" ID="ID_1667737141" MODIFIED="1455926631308" TEXT="Typ-Feld in geschachteltem Record"/>
<node CREATED="1455926632320" ID="ID_1323008809" MODIFIED="1455926641491" TEXT="Typ der Payload -&gt; Visitor"/>
</node>
<node CREATED="1455913759727" FOLDED="true" HGAP="43" ID="ID_1318158783" MODIFIED="1488423308078" TEXT="Modell">
<node CREATED="1455913778429" FOLDED="true" ID="ID_884750408" MODIFIED="1488423308056" TEXT="sub-Collections">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eindeutig &#252;berlegen
    </p>
    <ul>
      <li>
        faktorisiert sauber
      </li>
      <li>
        Zustand delegiert auf die jeweilige Kinder-Sammlung
      </li>
      <li>
        diese wird damit auch zum generischen Element
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1455913847683" ID="ID_1829117976" MODIFIED="1455913860029" TEXT="es gibt effektiv mehrere Kinder-Sammlungen"/>
<node CREATED="1455913860594" ID="ID_631826706" MODIFIED="1472829913272" TEXT="wir verwalten intern f&#xfc;r jede eine Position">
<arrowlink DESTINATION="ID_562014580" ENDARROW="Default" ENDINCLINATION="451;-8;" ID="Arrow_ID_543974195" STARTARROW="None" STARTINCLINATION="322;-32;"/>
</node>
<node CREATED="1455913877183" ID="ID_1295733719" MODIFIED="1455913906711" TEXT="forwarding by type"/>
</node>
<node CREATED="1455913785555" FOLDED="true" ID="ID_1777441123" MODIFIED="1488423308057" TEXT="transparenter Filter">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      schlechter....
    </p>
    <ul>
      <li>
        sammelt viel technische Komplexit&#228;t auf top-level
      </li>
      <li>
        wir m&#252;ssen eine meta-Repr&#228;sentation aufbauen
      </li>
      <li>
        wir m&#252;ssen Adapter zentral generieren, anstatt uns vom Installieren von Closures treiben zu lassen
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
<node CREATED="1455913912323" ID="ID_1775410993" MODIFIED="1455913922605" TEXT="es gibt nominell nur eine Kinder-Folge"/>
<node CREATED="1455913924857" ID="ID_505406246" MODIFIED="1455913935019" TEXT="wir m&#xfc;ssen die intern repr&#xe4;sentieren"/>
<node CREATED="1455913938287" FOLDED="true" ID="ID_749740013" MODIFIED="1488423306952" TEXT="passenden Adapter">
<node CREATED="1455913959452" ID="ID_1132015088" MODIFIED="1455913969855" TEXT="erst beim Zugriff"/>
<node CREATED="1455913970443" ID="ID_164083502" MODIFIED="1455913995371" TEXT="jeweils Typ feststellen"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node COLOR="#3a3785" CREATED="1455927009621" FOLDED="true" HGAP="148" ID="ID_153254427" MODIFIED="1488423342546" STYLE="fork" TEXT="&#xbb;generic tree diff&#xab;" VSHIFT="22">
<cloud COLOR="#edd8ba"/>
<font NAME="SansSerif" SIZE="15"/>
<node CREATED="1455927154554" FOLDED="true" ID="ID_1055035778" MODIFIED="1488423308112" TEXT="Prinzip">
<node CREATED="1455927162321" ID="ID_285833614" MODIFIED="1455927166972" TEXT="chained adapters"/>
<node CREATED="1455927169432" ID="ID_1685554012" MODIFIED="1455927207911" TEXT="inline closures"/>
<node CREATED="1455927216698" ID="ID_817434960" MODIFIED="1455927225756" TEXT="selector predicate"/>
</node>
<node CREATED="1455927270458" FOLDED="true" ID="ID_1471647972" MODIFIED="1488423308112" TEXT="Struktur">
<node CREATED="1455927276314" FOLDED="true" ID="ID_1425598677" MODIFIED="1488423308097" TEXT="Diff-verben">
<cloud COLOR="#fce9c0"/>
<font NAME="SansSerif" SIZE="16"/>
<node CREATED="1455927329507" FOLDED="true" ID="ID_949976479" MODIFIED="1488423308078" TEXT="pick">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927396505" ID="ID_54934717" MODIFIED="1455927399733" TEXT="verify"/>
<node CREATED="1455927413191" ID="ID_1313233235" MODIFIED="1455982912941" TEXT="accept_src"/>
<node CREATED="1455927425726" ID="ID_1010708624" MODIFIED="1455982690058" TEXT="next_src"/>
</node>
<node CREATED="1455927334882" FOLDED="true" ID="ID_724923414" MODIFIED="1488423308078" TEXT="ins">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927474671" ID="ID_739710454" MODIFIED="1455982918963" TEXT="inject"/>
</node>
<node CREATED="1455927341473" FOLDED="true" ID="ID_573257215" MODIFIED="1488423308078" TEXT="del">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927479054" ID="ID_93562485" MODIFIED="1455927482234" TEXT="verify"/>
<node CREATED="1455927483301" ID="ID_1149563805" MODIFIED="1470522011715" TEXT="skipSrc"/>
</node>
<node CREATED="1455927346728" FOLDED="true" ID="ID_1236186762" MODIFIED="1488423308078" TEXT="find">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927510666" ID="ID_968873842" MODIFIED="1455927514101" TEXT="locate_src"/>
<node CREATED="1455927522081" ID="ID_476699106" MODIFIED="1455982923915" TEXT="accept_src"/>
</node>
<node CREATED="1455927354239" FOLDED="true" ID="ID_501366037" MODIFIED="1488423308078" TEXT="skip">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1470522026709" ID="ID_967937679" MODIFIED="1470522029792" TEXT="skipSrc"/>
<node CREATED="1470522030780" ID="ID_628304064" MODIFIED="1470522072842" TEXT="Problem: welcher Layer?">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1455927357415" FOLDED="true" ID="ID_1730455685" MODIFIED="1488423308079" TEXT="after">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927556300" FOLDED="true" ID="ID_476640075" MODIFIED="1488423308057" TEXT="while not">
<node CREATED="1455927560940" ID="ID_1275606051" MODIFIED="1455982930194" TEXT="accept_src"/>
<node CREATED="1455982708749" ID="ID_355029700" MODIFIED="1455982708749" TEXT="next_src"/>
</node>
<node CREATED="1455927589560" FOLDED="true" ID="ID_1967720214" MODIFIED="1488423308057" TEXT="_ATTRIB_">
<node CREATED="1455927596055" ID="ID_829661861" MODIFIED="1455927600866" TEXT="attrib -&gt; end"/>
</node>
<node CREATED="1455927603566" FOLDED="true" ID="ID_604132662" MODIFIED="1488423308057" TEXT="_END_">
<node CREATED="1455927607621" ID="ID_948240245" MODIFIED="1455927610465" TEXT="all -&gt; end"/>
</node>
</node>
<node CREATED="1455927360398" FOLDED="true" ID="ID_1123321103" MODIFIED="1488423308079" TEXT="set">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927661046" FOLDED="true" ID="ID_608131093" MODIFIED="1488423308057" TEXT="if not verify">
<node CREATED="1455927681491" ID="ID_1621577646" MODIFIED="1455927694621" TEXT="locate_target"/>
</node>
<node CREATED="1455927697105" ID="ID_1909489348" MODIFIED="1455927701684" TEXT="verify"/>
<node CREATED="1455927702465" ID="ID_321172748" MODIFIED="1455927704244" TEXT="assign"/>
</node>
<node CREATED="1455927363102" FOLDED="true" ID="ID_1117221392" MODIFIED="1488423308079" TEXT="mut">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927714615" FOLDED="true" ID="ID_958571239" MODIFIED="1488423308057" TEXT="if not verify">
<node CREATED="1455927718574" ID="ID_1870238884" MODIFIED="1455927721586" TEXT="locate_target"/>
</node>
<node CREATED="1455927724238" ID="ID_664459993" MODIFIED="1455927734345" TEXT="verify"/>
<node CREATED="1455927736084" FOLDED="true" ID="ID_1788904482" MODIFIED="1488423308057" TEXT="push">
<node CREATED="1455927739355" ID="ID_1160724761" MODIFIED="1455927741512" TEXT="mutator"/>
</node>
</node>
<node CREATED="1455927365094" FOLDED="true" ID="ID_1567754459" MODIFIED="1488423308079" TEXT="emu">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927745131" ID="ID_1000032658" MODIFIED="1455927749590" TEXT="verify_end"/>
<node CREATED="1455927750490" ID="ID_1746587224" MODIFIED="1455927752838" TEXT="pop"/>
</node>
<node CREATED="1455927337321" FOLDED="true" ID="ID_1622487857" MODIFIED="1488423308079" TEXT="CTOR">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455927444275" FOLDED="true" ID="ID_643592318" MODIFIED="1488423308057" TEXT="push">
<node CREATED="1455927462641" ID="ID_14883638" MODIFIED="1455927465084" TEXT="mutator"/>
</node>
</node>
</node>
<node CREATED="1455927276314" FOLDED="true" HGAP="91" ID="ID_1209647803" MODIFIED="1488423308097" VSHIFT="-3">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Primitive
    </p>
    <p>
      <font size="2">(impl-ops)</font>
    </p>
  </body>
</html></richcontent>
<cloud COLOR="#fce9c0"/>
<font NAME="SansSerif" SIZE="16"/>
<node CREATED="1455982947867" FOLDED="true" ID="ID_1339677569" MODIFIED="1488423308079" TEXT="injectNew">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455982969073" ID="ID_1885889112" MODIFIED="1455982974332" TEXT="inject new content"/>
<node CREATED="1457047512175" ID="ID_1126383522" MODIFIED="1457047519426" TEXT="at implicit &quot;current&quot; position"/>
</node>
<node CREATED="1455927425726" FOLDED="true" ID="ID_1776437339" MODIFIED="1488423308079" TEXT="hasSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928216420" ID="ID_662720483" MODIFIED="1464117064570" TEXT="further src elements available"/>
<node CREATED="1470527053136" ID="ID_1383280265" MODIFIED="1470527075712">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      of questionable use
    </p>
    <p>
      with multiple layers
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1455927396505" FOLDED="true" ID="ID_392033275" MODIFIED="1488423308079" TEXT="matchSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928268589" FOLDED="true" ID="ID_97473072" MODIFIED="1488423308058" TEXT="ID comparison">
<node CREATED="1455928524530" ID="ID_545057240" MODIFIED="1457120284424" TEXT="implicit next pos"/>
<node CREATED="1455928530738" ID="ID_1035043901" MODIFIED="1455928533677" TEXT="ID"/>
</node>
<node CREATED="1470778603801" ID="ID_1808045935" MODIFIED="1472219338431" TEXT="needed to implement the `del` verb">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      since skipSrc performs both the `del` and the `skip` verb, it can not perform the match itself...
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1455927425726" FOLDED="true" ID="ID_1759686725" MODIFIED="1488423308079" TEXT="skipSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1470527086660" ID="ID_13765501" MODIFIED="1470527097135" TEXT="guarded by selector">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1470772440180" ID="ID_1241607377" MODIFIED="1472219338440" TEXT="can not match by itself">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...because it is also used to discard garbage after a findSrc operation.
    </p>
    <p>
      Thus we need to avoid touching the actual data in the src sequence, because this might lead to SEGFAULT.
    </p>
    <p>
      
    </p>
    <p>
      For this reason, the implementation of the `del` verb has to invoke matchSrc explicitly beforehand,
    </p>
    <p>
      and this is the very reason `matchSrc` exists. Moreover, `matchSrc` must be written such
    </p>
    <p>
      as to ensure to invoke the Selector before performing a local match. And skipSrc has to
    </p>
    <p>
      proceed in precisely the same way. Thus, if the selector denies responsibility, we'll delegate
    </p>
    <p>
      to the next lower layer in both cases, and the result and behaviour depends on this next lower layer solely
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455928216420" ID="ID_1581600385" MODIFIED="1470778395226" TEXT="thus just advance source position"/>
</node>
<node CREATED="1455927413191" FOLDED="true" ID="ID_1624797970" MODIFIED="1488423308079" TEXT="acceptSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1470527086660" ID="ID_410793564" MODIFIED="1470772489007" TEXT="guarded by selector">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1470772440180" ID="ID_836603881" MODIFIED="1470772503839" TEXT="verify match with next src position"/>
<node CREATED="1455928275316" FOLDED="true" ID="ID_702364156" MODIFIED="1488423308058">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      then <i>move</i>&#160;into target
    </p>
  </body>
</html></richcontent>
<node CREATED="1455928537273" ID="ID_1036724915" MODIFIED="1457120296935" TEXT="implicit next pos"/>
</node>
</node>
<node CREATED="1455928166683" FOLDED="true" ID="ID_238485247" MODIFIED="1488423308079" TEXT="accept_until">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928383765" ID="ID_522592769" MODIFIED="1455928389737" TEXT="accept until condition is met"/>
<node CREATED="1457231714541" ID="ID_1234563594" MODIFIED="1457231725704" TEXT="ID spec"/>
<node CREATED="1472829929343" ID="ID_797835614" MODIFIED="1472829944881" TEXT="special handling for Ref::END / Ref::ATTRIBS"/>
<node CREATED="1457231524839" ID="ID_1492447432" MODIFIED="1472219338456" TEXT="needs to establish responsible target beforehand">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      since, on interface level, we're pretending that this mutator <i>is a single collection like thing,</i>
    </p>
    <p>
      while in fact the implementation might bind to several opaque target structures.
    </p>
    <p>
      Thus, internally we'll have a <b>selector</b>&#160;to determine which onion layer is responsible for
    </p>
    <p>
      handling an element as designated by the argument. It is then the responsibility
    </p>
    <p>
      of this specific onion layer to accept forward until meeting this element.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1457231727259" ID="ID_1318527107" MODIFIED="1472219338462">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#e72d0e">warning:</font>&#160;messed-up state in case of failure
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      this is (probably) the only operation which entirely messes up the mutator state
    </p>
    <p>
      when the designated target does not exist. The assumption is that a diff application front-end
    </p>
    <p>
      will check the bool return value and throw an exception in that case
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1455927413191" FOLDED="true" ID="ID_94560199" MODIFIED="1488423308080" TEXT="findSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1461946965569" ID="ID_803034273" MODIFIED="1461946980872" TEXT="locate designated src element"/>
<node CREATED="1455928275316" FOLDED="true" ID="ID_1937317223" MODIFIED="1488423308058">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>move</i>&#160;into target
    </p>
  </body>
</html></richcontent>
<node CREATED="1455928537273" ID="ID_1177385447" MODIFIED="1457120296935" TEXT="implicit next pos"/>
</node>
</node>
<node CREATED="1455928184504" FOLDED="true" ID="ID_836844042" MODIFIED="1488423308080" TEXT="assignElm">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928393788" FOLDED="true" ID="ID_403756991" MODIFIED="1488423308058" TEXT="locate target">
<node CREATED="1457190206827" ID="ID_1711690131" MODIFIED="1457190220485" TEXT="allready accepted element"/>
<node CREATED="1457190227089" ID="ID_1094585247" MODIFIED="1457190234435" TEXT="find by ID comparison"/>
<node CREATED="1457190235191" ID="ID_926146761" MODIFIED="1457190240027" TEXT="try current element first"/>
</node>
<node CREATED="1455928403931" FOLDED="true" ID="ID_1750063931" MODIFIED="1488423308058" TEXT="assign payload">
<node CREATED="1455928586506" ID="ID_1163700862" MODIFIED="1457190259104" TEXT="implicit Type context"/>
<node CREATED="1455928612239" ID="ID_588352525" MODIFIED="1455928619186" TEXT="embedded target data"/>
</node>
</node>
<node CREATED="1455928189695" FOLDED="true" ID="ID_1319966604" MODIFIED="1488423308080" TEXT="mutateChild">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1457190279001" FOLDED="true" ID="ID_1000897100" MODIFIED="1488423308058" TEXT="locate target">
<node CREATED="1457190288624" ID="ID_1258290029" MODIFIED="1457190296115" TEXT="same as in &apos;assignElm&apos;"/>
</node>
<node CREATED="1455928415161" FOLDED="true" ID="ID_1664558338" MODIFIED="1488423308058" TEXT="fabricate mutator">
<node CREATED="1457190370846" ID="ID_1146518798" MODIFIED="1457190374749" TEXT="suitable for target"/>
<node CREATED="1455928625269" ID="ID_1880024064" MODIFIED="1457190388167" TEXT="open / init mutator"/>
</node>
<node CREATED="1457190320444" FOLDED="true" ID="ID_1835423331" MODIFIED="1488423308058" TEXT="place into provided buffer">
<node CREATED="1457190396170" ID="ID_275107868" MODIFIED="1457190419158">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>throw </i>when
    </p>
    <p>
      insufficent space
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1464113688645" FOLDED="true" ID="ID_954516697" MODIFIED="1488423308080" STYLE="fork" TEXT="completeScope">
<edge COLOR="#808080" STYLE="bezier" WIDTH="thin"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1464113727192" ID="ID_357369611" MODIFIED="1464113739514" TEXT="pr&#xfc;ft, ob alles eingel&#xf6;st wurde"/>
<node CREATED="1464113740005" ID="ID_1649881726" MODIFIED="1464113748425" TEXT="em&#xf6;glicht Aufr&#xe4;um-Arbeiten"/>
<node CREATED="1464113754412" ID="ID_1095488369" MODIFIED="1472219338490" TEXT="kann NOP implementiert werden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...in F&#228;llen, in denen der konkrete onion-layer
    </p>
    <p>
      &#252;berhaupt nicht im Stande ist, das zu beurteilen.
    </p>
    <p>
      Wichtigster solcher Fall ist die <b>Bindung auf Objekt-Felder</b>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node COLOR="#e5500c" CREATED="1461946693701" FOLDED="true" ID="ID_36720859" MODIFIED="1488423308080" STYLE="fork" TEXT="meta-Operationen">
<edge COLOR="#808080" STYLE="bezier" WIDTH="thin"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928193863" FOLDED="true" ID="ID_1750104896" MODIFIED="1488423308058" TEXT="CTOR">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928427432" ID="ID_1760631999" MODIFIED="1461946914155" TEXT="take ownership of existing contents"/>
<node CREATED="1461946885292" ID="ID_1743121055" MODIFIED="1461946907948" TEXT="move them into source buffer"/>
</node>
<node CREATED="1455928193863" FOLDED="true" ID="ID_545402083" MODIFIED="1488423308058" TEXT="open_subScope">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928427432" ID="ID_1189452497" MODIFIED="1455928435275" TEXT="push mutator on stack"/>
<node CREATED="1457190622236" ID="ID_1347446227" MODIFIED="1461946862429">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      invoke <b>mutateChild</b>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1455928198718" FOLDED="true" ID="ID_1546579792" MODIFIED="1488423308059" TEXT="close_subScope">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928440366" ID="ID_33734026" MODIFIED="1457190669578" TEXT="just pop mutator"/>
<node CREATED="1457190670069" ID="ID_242879400" MODIFIED="1472219338544" TEXT="abandon mutator">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>NOTE</b>: mutator need to be written in such a way
    </p>
    <p>
      to be just discarded when done with the alterations.
    </p>
    <p>
      That is, the mutator<i>&#160;must not</i>&#160;incorporate the target data, rather it is expected
    </p>
    <p>
      to construct the new target data efficiently in place.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1457190826145" ID="ID_1342883622" MODIFIED="1457190834604" TEXT="return to previous mutator"/>
</node>
</node>
</node>
</node>
</node>
<node COLOR="#85212c" CREATED="1455928782528" FOLDED="true" HGAP="-171" ID="ID_1300146872" MODIFIED="1488423342546" TEXT="kritische Aufgaben" VSHIFT="-207">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1455928917782" FOLDED="true" HGAP="40" ID="ID_812598653" MODIFIED="1488423308112" TEXT="failure Policy" VSHIFT="-11">
<font NAME="SansSerif" SIZE="15"/>
<icon BUILTIN="idea"/>
<node CREATED="1455928937683" ID="ID_784708305" MODIFIED="1455928953041" TEXT="ignorieren">
<icon BUILTIN="full-1"/>
</node>
<node CREATED="1455928949346" ID="ID_1044467237" MODIFIED="1455928955415" TEXT="scheitern">
<icon BUILTIN="full-2"/>
</node>
</node>
<node CREATED="1455928800670" FOLDED="true" HGAP="83" ID="ID_1116973935" MODIFIED="1488423308112" TEXT="selector predicate" VSHIFT="-5">
<node CREATED="1455928838865" ID="ID_1505861137" MODIFIED="1455928845027" TEXT="konfigurierbarer Typ-Check"/>
<node CREATED="1455928846048" ID="ID_1278060510" MODIFIED="1455928856146" TEXT="TypCheck nur einmal"/>
<node CREATED="1455928856870" ID="ID_806857541" MODIFIED="1455928878335" TEXT="ggfs Sequenz von ID-Checks"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1455928805717" FOLDED="true" HGAP="76" ID="ID_496821508" MODIFIED="1488423308112" TEXT="mutator allocation" VSHIFT="-1">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1455930962188" ID="ID_1435085503" MODIFIED="1455930970295" TEXT="konkreter Mutator-Typ"/>
<node CREATED="1455930971027" ID="ID_1070880241" MODIFIED="1455930980789" TEXT="ist in jedem Einzelfall anders"/>
<node CREATED="1455931008750" ID="ID_643730154" MODIFIED="1455931124521" TEXT="Mutator enth&#xe4;lt Closures">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Mutator enth&#228;lt die Bindung auf die konkreten Daten
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455930997303" ID="ID_1710972681" MODIFIED="1455931003962" TEXT="kann erhebliche Gr&#xf6;&#xdf;e haben"/>
<node CREATED="1455931025595" ID="ID_1695885971" MODIFIED="1455931100164" TEXT="ist a priori nicht bekannt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      stellt sich u.U erst w&#228;hrend der Verarbeitung heraus:
    </p>
    <p>
      bei &quot;offenen Datenstrukturen&quot; entscheided jeder Typ selber,
    </p>
    <p>
      welchen Mutator er erzeugt
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456011474920" FOLDED="true" HGAP="-25" ID="ID_1292432889" MODIFIED="1488423308097" TEXT="L&#xf6;sungsans&#xe4;tze" VSHIFT="28">
<icon BUILTIN="idea"/>
<node CREATED="1456011490414" FOLDED="true" ID="ID_215912114" MODIFIED="1488423308080" TEXT="doch rekursiv">
<icon BUILTIN="button_cancel"/>
<node CREATED="1456011497525" ID="ID_165091928" MODIFIED="1456011507880" TEXT="elegant"/>
<node CREATED="1456011508731" ID="ID_1951843758" MODIFIED="1456011513711" TEXT="Allokation nebenbei"/>
<node CREATED="1456011591427" FOLDED="true" ID="ID_167337651" MODIFIED="1488423308059">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aber:&#160;Aufrufprinzip
    </p>
  </body>
</html></richcontent>
<node CREATED="1456011595024" ID="ID_1268079925" MODIFIED="1456011598819" TEXT="mu&#xdf; konsumieren"/>
<node CREATED="1456011599455" ID="ID_862262812" MODIFIED="1456011634435">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Verb mu&#223; den
    </p>
    <p>
      Diff bekommen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456011647057" ID="ID_905672765" MODIFIED="1456011652964" TEXT="verwirrendes API"/>
<node CREATED="1456011653904" ID="ID_1199841076" MODIFIED="1456011656396" TEXT="komplexe Typen"/>
<node CREATED="1456011660495" ID="ID_118747856" MODIFIED="1456011661939" TEXT="njet"/>
</node>
</node>
<node CREATED="1456011666486" FOLDED="true" ID="ID_1076199754" MODIFIED="1488423308080" TEXT="Interpreter konsumiert">
<icon BUILTIN="button_cancel"/>
<node CREATED="1456011676086" ID="ID_1849622483" MODIFIED="1456011688340">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und delegiert iterativ
    </p>
    <p>
      an die Verben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456011689043" ID="ID_1674393396" MODIFIED="1456011698214" TEXT="mu&#xdf; Verben untersuchen"/>
<node CREATED="1456011698970" ID="ID_845834851" MODIFIED="1456011717387" TEXT="aktiv in Rekursion einsteigen"/>
<node CREATED="1456011717951" ID="ID_1167483025" MODIFIED="1456011722666" TEXT="auch verwirrend"/>
</node>
<node CREATED="1456011734253" FOLDED="true" ID="ID_478177524" MODIFIED="1488423308080" TEXT="doch beim Stack bleiben">
<icon BUILTIN="help"/>
<node CREATED="1456011910869" ID="ID_182841484" MODIFIED="1456429740520" TEXT="kleineres &#xdc;bel">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1456011918381" ID="ID_54237513" MODIFIED="1456011991777" TEXT="Rolle des Mutators">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1456012543585" FOLDED="true" HGAP="26" ID="ID_1774389094" MODIFIED="1488423308080" TEXT="Struktur-Ideen" VSHIFT="20">
<node CREATED="1456012563942" ID="ID_842130823" MODIFIED="1456012570081" TEXT="Referenz zum Mutator"/>
<node CREATED="1456012572045" ID="ID_340104657" MODIFIED="1456012577752" TEXT="Mutator ist smart-ptr"/>
<node CREATED="1456012578868" ID="ID_1107535684" MODIFIED="1456012585103" TEXT="Frontend-Backend"/>
<node CREATED="1456012585835" ID="ID_1377280414" MODIFIED="1456012605260" TEXT="Mutator im Inline-Puffer"/>
<node CREATED="1456012665681" ID="ID_227844194" MODIFIED="1456012704695" TEXT="Gr&#xf6;&#xdf;e initial festlegen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn in dem Moment, wo wir den top-level TreeMutator erzeugen,
    </p>
    <p>
      k&#246;nnen wir rekursiv abfragen, wie gro&#223; alle m&#246;glichen Kind-Mutatoren werden k&#246;nnen
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#d8bba4" CREATED="1456011994442" FOLDED="true" HGAP="68" ID="ID_343687995" MODIFIED="1488423308112" TEXT="zur Entscheidung..." VSHIFT="37">
<edge COLOR="#93766f"/>
<arrowlink COLOR="#b10046" DESTINATION="ID_820279076" ENDARROW="Default" ENDINCLINATION="14;-46;" ID="Arrow_ID_103087571" STARTARROW="None" STARTINCLINATION="-137;-20;"/>
<font NAME="SansSerif" SIZE="15"/>
<icon BUILTIN="idea"/>
<node CREATED="1456012846185" FOLDED="true" ID="ID_714953132" MODIFIED="1488423308097" TEXT="was ist der Mutator">
<icon BUILTIN="help"/>
<node CREATED="1456012035837" ID="ID_180428099" MODIFIED="1456012039389" TEXT="Container"/>
<node CREATED="1456184290492" ID="ID_1860454535" MODIFIED="1456184298524" TEXT="Strategy"/>
<node CREATED="1456012028134" ID="ID_820709337" MODIFIED="1456012031953" TEXT="Adapter"/>
<node CREATED="1456012033085" ID="ID_755204065" MODIFIED="1456012034841" TEXT="Puffer"/>
</node>
<node CREATED="1456012859031" FOLDED="true" ID="ID_1120123290" MODIFIED="1488423308097" TEXT="wer erzeugt den Mutator">
<icon BUILTIN="help"/>
<node CREATED="1456013036967" ID="ID_1719719197" MODIFIED="1456013042330" TEXT="der Applikator"/>
<node CREATED="1456013043038" ID="ID_1263902491" MODIFIED="1456013047497" TEXT="die Zieldatenstruktur"/>
</node>
<node CREATED="1456013049909" FOLDED="true" HGAP="107" ID="ID_1581320145" MODIFIED="1488423308097" TEXT="Anwendungs-F&#xe4;lle" VSHIFT="29">
<cloud COLOR="#d6c6a5"/>
<font NAME="SansSerif" SIZE="16"/>
<node CREATED="1456013057420" FOLDED="true" ID="ID_1458089109" MODIFIED="1488423308080" TEXT="externe Datenstruktur">
<node CREATED="1456013290573" ID="ID_802953483" MODIFIED="1456013297208" TEXT="hat beliebig Platz"/>
<node CREATED="1456013298084" ID="ID_117703425" MODIFIED="1456013305071" TEXT="Closures in der Impl-Klasse"/>
<node CREATED="1456013306186" ID="ID_1397201984" MODIFIED="1456013330235" TEXT="effizient nur ohne Trennung"/>
<node CREATED="1456013359452" ID="ID_675424172" MODIFIED="1456013364783" TEXT="braucht Hilfe beim Lebenszyklus"/>
<node CREATED="1456013368426" ID="ID_1906604960" MODIFIED="1456013373717" TEXT="k&#xf6;nnte Puffer selber allozieren"/>
</node>
<node CREATED="1456013068179" FOLDED="true" ID="ID_774826163" MODIFIED="1488423308081" TEXT="Rec&lt;GenNode&gt;">
<node CREATED="1456013114140" ID="ID_1057946691" MODIFIED="1456013123199" TEXT="hat keinen extra Platz"/>
<node CREATED="1456013123739" ID="ID_1081025245" MODIFIED="1456013132837" TEXT="Tick: in RecMutator umdeuten"/>
<node CREATED="1456013133386" ID="ID_1417100176" MODIFIED="1456013138077" TEXT="Puffer mu&#xdf; woanders leben"/>
<node CREATED="1456013149327" ID="ID_687443349" MODIFIED="1456013168385" TEXT="Impl-Collection in Puffer verschieben"/>
</node>
<node CREATED="1456013063011" FOLDED="true" ID="ID_908970422" MODIFIED="1488423308081" TEXT="Collection">
<node CREATED="1456013202760" ID="ID_1619699608" MODIFIED="1456013218586" TEXT="hat keinen extra Platz"/>
<node CREATED="1456013219750" ID="ID_1766379648" MODIFIED="1456013227785" TEXT="kann &#xfc;berhaupt nicht helfen"/>
<node CREATED="1456013231117" ID="ID_1899116139" MODIFIED="1456013238279" TEXT="Puffer mu&#xdf; woanders leben"/>
<node CREATED="1456013239820" ID="ID_1794944692" MODIFIED="1456013244911" TEXT="wieder Verschiebe-Trick"/>
</node>
</node>
<node CREATED="1456184583336" FOLDED="true" HGAP="125" ID="ID_1225582084" MODIFIED="1488423308098" TEXT="weitere Beobachtungen" VSHIFT="36">
<icon BUILTIN="forward"/>
<node CREATED="1456184689466" FOLDED="true" ID="ID_1635861686" MODIFIED="1488423308081" TEXT="externe Struktur == Ausnahme">
<node CREATED="1456184725333" ID="ID_646480978" MODIFIED="1456184735783" TEXT="unterst&#xfc;tzt keine Umordnung"/>
<node CREATED="1456184736827" ID="ID_171431775" MODIFIED="1456184746398" TEXT="unterst&#xfc;tzt kein Hinzuf&#xfc;gen"/>
<node CREATED="1456184746946" ID="ID_1768153345" MODIFIED="1456184756780" TEXT="L&#xf6;schen wird uminterpretiert"/>
</node>
<node CREATED="1456184697944" FOLDED="true" ID="ID_1904390217" MODIFIED="1488423308081" TEXT="Mutator und subMutator">
<node CREATED="1456184816553" ID="ID_1019484567" MODIFIED="1456184832218" TEXT="getypte Kinder -&gt; subMutator"/>
<node CREATED="1456184833446" ID="ID_1632860381" MODIFIED="1456184864038" TEXT="subMutator f&#xfc;r subCollection"/>
<node CREATED="1461881226351" ID="ID_211513452" MODIFIED="1461881233506" TEXT="subMutator == &quot;onon layer&quot;"/>
<node CREATED="1456184892870" ID="ID_1216247694" MODIFIED="1456184905433" TEXT="Rec&lt;GenNode&gt; -&gt; zwei subCollections"/>
</node>
<node CREATED="1456186634006" FOLDED="true" ID="ID_310265994" MODIFIED="1488423308081" TEXT="Profil des Mutators">
<node CREATED="1456186654411" ID="ID_1341247857" MODIFIED="1456186678780" TEXT="Erzeugen geht schnell">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nur Zuweisung einiger Referenzen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456186688671" FOLDED="true" ID="ID_1110994752" MODIFIED="1488423308059" TEXT="Allokation">
<node CREATED="1456186753094" ID="ID_653529641" MODIFIED="1456186771647" TEXT="Gr&#xf6;&#xdf;enordnung der Zieldatenstruktur"/>
<node CREATED="1456186772163" ID="ID_1555284305" MODIFIED="1456186779750" TEXT="im Collection-Fall kann man durchreichen"/>
</node>
<node CREATED="1456186797568" ID="ID_1773938090" MODIFIED="1456186809890" TEXT="ausschlie&#xdf;lich an die Diff-Anwendung gekoppelt"/>
<node CREATED="1456186915112" FOLDED="true" ID="ID_1560470636" MODIFIED="1488423308059" TEXT="Gr&#xf6;&#xdf;e duch Binde-Voragang bestimmt">
<node CREATED="1456186933998" FOLDED="true" ID="ID_528314478" MODIFIED="1488423306971" TEXT="Zahl der subMutatoren">
<node CREATED="1456187166294" ID="ID_1282142171" MODIFIED="1456187174186" TEXT="ein Vector pro Collection"/>
<node CREATED="1456416605491" ID="ID_1722541208" MODIFIED="1456416614478" TEXT="eigentlicher Content wird durchgereicht"/>
</node>
<node CREATED="1456186946508" FOLDED="true" ID="ID_335779075" MODIFIED="1488423306972" TEXT="Zahl der Einzelbindungen">
<node CREATED="1456187264346" ID="ID_81725454" MODIFIED="1456187278539" TEXT="ein Slot pro Binding-Layer"/>
<node CREATED="1456187279408" FOLDED="true" ID="ID_954788495" MODIFIED="1488423306972" TEXT="Attribut-Map">
<node CREATED="1456187295550" ID="ID_534521654" MODIFIED="1456187329981" TEXT="Setter und Getter"/>
<node CREATED="1456187330641" ID="ID_1069743778" MODIFIED="1456187335684" TEXT="zwei Slots pro Attribut"/>
<node CREATED="1456416534525" ID="ID_1195638113" MODIFIED="1456416573868" TEXT="+Storage f&#xfc;r den Key">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....denn der liegt (mind) einmal vor,
    </p>
    <p>
      eingebettet in ein Selektor-Pr&#228;dikat,
    </p>
    <p>
      welches bestimmt, ob dieses Attribut angesprochen wird
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456187338288" ID="ID_1194536240" MODIFIED="1456187339692" TEXT="Heap"/>
</node>
</node>
</node>
</node>
<node CREATED="1456437116514" FOLDED="true" ID="ID_1766577674" MODIFIED="1488423308081" TEXT="Reflexion">
<cloud COLOR="#d2c2d9"/>
<node CREATED="1456187434491" ID="ID_860365577" MODIFIED="1456437137837" TEXT="alles ekelhaft">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1456187447618" ID="ID_1318283286" MODIFIED="1456187459568" TEXT="Sackgasse">
<icon BUILTIN="help"/>
</node>
<node CREATED="1456424184807" FOLDED="true" ID="ID_770700785" MODIFIED="1488423308059" TEXT="Alternativen">
<icon BUILTIN="help"/>
<node CREATED="1456424237672" ID="ID_811857290" MODIFIED="1456424410221" TEXT="zu Fu&#xdf; programmieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      was man konventionellerweise auch macht.
    </p>
    <p>
      Ich verstehe nun, warum. Es ist der <i>vern&#252;nftigste</i>&#160;Weg.
    </p>
    <p>
      
    </p>
    <p>
      Leider scheidet das aber f&#252;r uns hier genau aus,
    </p>
    <p>
      denn das gesamte Projekt entstand, aufgrund der inh&#228;renten Limitierungen
    </p>
    <p>
      der &quot;vern&#252;nftigen&quot; (=pragmatischen) L&#246;sung.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456424257381" ID="ID_740973686" MODIFIED="1456424577215" TEXT="introspektive Datenstruktur">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      dieser Ansatz l&#246;st tats&#228;chlich das Problem,
    </p>
    <p>
      aber zu dem Preis, da&#223; er die Strukturen von innen her zersetzt.
    </p>
    <p>
      Auf lange Sicht wird das System wuchern wie ein Krebsgeschw&#252;hr,
    </p>
    <p>
      und man kann das nur mit Disziplin eind&#228;mmen, was <i>realistisch gesprochen</i>&#160;meint,
    </p>
    <p>
      da&#223; es vergeblich ist. Einen Kampf gegen das Menschliche, Allzumenschliche kann man nicht gewinnen.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456424962239" FOLDED="true" ID="ID_933613304" MODIFIED="1488423306972" TEXT="einseitige Fernsteuerung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das ist die schlankeste L&#246;sung, die &#228;sthetisch befriedigt.
    </p>
    <p>
      Sie hat aber das Problem, da&#223; dadurch die Kollaboration im Kern ausgel&#246;scht wird.
    </p>
    <p>
      Wir haben eine Seite, die absolute Macht hat, und einen &quot;Partner&quot;, der tats&#228;chlich nur ferngesteuert ist.
    </p>
    <p>
      Wir m&#252;ssen daf&#252;r auf die Subsidiarit&#228;t verzichten, und damit auf die M&#246;glichkeit zur Entkoppelung.
    </p>
    <p>
      Dazu kommt, da&#223; die notwendige Fern-Wirkung stets eine zus&#228;tzliche Last bedeutet.
    </p>
    <p>
      Denn wir m&#252;ssen auf Umst&#228;nde und Strukturen einwirken, die von dem Ort, an dem die
    </p>
    <p>
      Steuerung stattfindet, entfernt ist, entfernt in einen anderen Kontext.
    </p>
  </body>
</html></richcontent>
<node CREATED="1456425646332" ID="ID_897964875" MODIFIED="1456425651999" TEXT="oberfl&#xe4;chengetrieben"/>
<node CREATED="1456425652683" ID="ID_437402611" MODIFIED="1456425656575" TEXT="zentralgesteuert"/>
</node>
<node CREATED="1456424295296" FOLDED="true" ID="ID_791299475" MODIFIED="1488423306973" TEXT="DOM + Attribution">
<icon BUILTIN="idea"/>
<node CREATED="1456424627860" ID="ID_1294496825" MODIFIED="1456424632247" TEXT="keine Alternative"/>
<node CREATED="1456424632907" ID="ID_1566098600" MODIFIED="1456424642598" TEXT="denn genau das machen wir hier"/>
<node CREATED="1456437181070" FOLDED="true" HGAP="23" ID="ID_1066542027" MODIFIED="1456437536212" STYLE="bubble" TEXT="Analyse" VSHIFT="44">
<font BOLD="true" NAME="SansSerif" SIZE="15"/>
<icon BUILTIN="info"/>
<node CREATED="1456424664015" FOLDED="true" ID="ID_1032924247" MODIFIED="1488423306972" TEXT="Problem: Zusatz-Kosten">
<node CREATED="1456424675173" ID="ID_1585536570" MODIFIED="1456437520768" TEXT="...f&#xfc;r das &quot;+&quot;"/>
<node CREATED="1456424695123" ID="ID_331589423" MODIFIED="1456437520769" TEXT="also die Bindung"/>
<node CREATED="1456424722039" FOLDED="true" ID="ID_1926017913" MODIFIED="1488423306972" TEXT="notwendige Kosten">
<node CREATED="1456424725927" ID="ID_1322872728" MODIFIED="1456437520769" TEXT="denn das DOM ist rein"/>
<node CREATED="1456424730022" ID="ID_282791744" MODIFIED="1456437520769" TEXT="hat also keinen Platz f&#xfc;r explizite Eigenschaften"/>
<node CREATED="1456424765113" ID="ID_1716539711" MODIFIED="1456437520770">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und nur letztere sind <i>tangibel</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1456425913664" FOLDED="true" HGAP="28" ID="ID_1929919601" MODIFIED="1488423306972" TEXT="notwendig" VSHIFT="-1">
<node CREATED="1456426041626" FOLDED="true" ID="ID_101262346" MODIFIED="1488423306972" TEXT="Duplikation">
<node CREATED="1456425918071" ID="ID_18884187" MODIFIED="1456437520772" TEXT="DOM-IDs am Binde-Punkt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      um den Binde- bzw. Ankn&#252;pfungs-Punkt in den real-Daten &#252;berhaupt zu finden,
    </p>
    <p>
      m&#252;ssen IDs aus dem DOM innerhalb der real-Daten nochmal wiederholt, also redundant vorliegen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456425930718" ID="ID_1301765327" MODIFIED="1456437520772" TEXT="real-Daten im DOM">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      damit das DOM ein echtes DOM ist, mu&#223; es die relevanten real-Daten duplizieren,
    </p>
    <p>
      um sie in einem abstrahierten Kontext zug&#228;nglich zu machen
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1456426052973" ID="ID_239653128" MODIFIED="1456437520772" TEXT="Binde-Daten">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      noch zus&#228;tzlich zur genannten Duplikation mu&#223;
    </p>
    <p>
      die Abblidung der Strukturen aufeinander
    </p>
    <p>
      an irgend einer Stelle repr&#228;sentiert werden.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456426322482" ID="ID_1330880148" MODIFIED="1456437520773" TEXT="irreduzibel">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      man kann versuchen, die beiden Elemente der Duplikation aufzul&#246;sen.
    </p>
    <p>
      Allerdings gibt es daf&#252;r &#252;berhaupt nur zwei m&#246;gliche Richtungen.
    </p>
    <ul>
      <li>
        man l&#246;st die Parallel-Strukturen auf
      </li>
      <li>
        man ersetzt das DOM durch reine Bindungs-Strukturen
      </li>
    </ul>
    <p>
      Beide Ans&#228;tze laufen aber auf eine der schon genanten, anderen Alternativen hinaus.
    </p>
    <p>
      Wenn man die Parallel-Strukturen beseitigt, enden wir bei irgend einer Form von Fernsteuerung.
    </p>
    <p>
      Wenn man die Modell-Natur aus dem DOM entfernt, das hei&#223;t, dort nur noch reine<br />Binde-Strukturen speichert, dann endet man bei einer Form von Introspektion. Entweder,
    </p>
    <p>
      das R&#252;ckgrat und die Navigation verbleibt bei dieser Introspektion; dann haben wir eines
    </p>
    <p>
      der typischen Objekt-Systeme. Oder die Binde-Daten werden zu einem reinen Anhang
    </p>
    <p>
      an eine selbst&#228;ndig bestehende Datenstruktur; dann enden wir bei klassischer Introspektion.
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1456428989206" FOLDED="true" HGAP="28" ID="ID_1999101531" MODIFIED="1488423306973" TEXT="Ma&#xdf;stab" VSHIFT="17">
<node CREATED="1456429002332" FOLDED="true" ID="ID_294696563" MODIFIED="1488423306972" TEXT="Trennung">
<node CREATED="1456429013283" ID="ID_462304497" MODIFIED="1456437520773" TEXT="abstrakt"/>
<node CREATED="1456429016050" ID="ID_403266218" MODIFIED="1456437520774" TEXT="tangibel"/>
</node>
<node CREATED="1456429046358" FOLDED="true" ID="ID_327619080" MODIFIED="1488423306972" TEXT="Fokussierung innerhalb">
<node CREATED="1456429106894" ID="ID_345392088" MODIFIED="1456437520774" TEXT="Belange des UI"/>
<node CREATED="1456429110702" ID="ID_1531485619" MODIFIED="1456437520774" TEXT="Notwendigkeiten des Edit-Vorganges"/>
<node CREATED="1456429159559" ID="ID_4036488" MODIFIED="1456437520774" TEXT="nach der inh&#xe4;renten Form suchen"/>
</node>
<node CREATED="1456429053206" ID="ID_643637016" MODIFIED="1456437520774" TEXT="wenig &#xdc;berlapp"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1456423477045" FOLDED="true" HGAP="146" ID="ID_1294023197" MODIFIED="1488423308098" TEXT="Stand der &#xdc;berlegungen" VSHIFT="11">
<icon BUILTIN="forward"/>
<node CREATED="1456423615274" FOLDED="true" ID="ID_40789197" MODIFIED="1488423308081" TEXT="Performance">
<node CREATED="1456423617763" ID="ID_524247634" MODIFIED="1456423622197" TEXT="wird von der Allokation bestimmt"/>
<node CREATED="1456423642687" ID="ID_1252228590" MODIFIED="1456423660940">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      reflektiert die Zahl der <i>Struktur</i>-Element
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456423661669" ID="ID_1064355256" MODIFIED="1456423805328" TEXT="Collections sind au&#xdf;en vor">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...will sagen:
    </p>
    <p>
      f&#252;r die habe ich bereits eine effiziente Implementierung,
    </p>
    <p>
      die darauf beruht, den Content beiseite zu schieben.
    </p>
    <p>
      Ich brauche also nur ein Container-Frontend (z.B. einen Vector ohne Inhalt) zus&#228;tzlich,
    </p>
    <p>
      um den verschobenen Inhalt erst mal aufzunehmen.
    </p>
    <p>
      Also z&#228;hlen Kinder-Collections nur als <i>ein </i>Strukturelement.
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1456423839637" FOLDED="true" ID="ID_1087136823" MODIFIED="1488423308081" TEXT="Platzbedarf">
<node CREATED="1456423848028" ID="ID_212325084" MODIFIED="1456423851935" TEXT="ist signifikant"/>
<node CREATED="1456423860130" ID="ID_1592353572" MODIFIED="1456423872404" TEXT="Gr&#xf6;&#xdf;enordnung der Ziel-Struktur (ohne Collections)"/>
<node CREATED="1456423883159" ID="ID_229501623" MODIFIED="1456423918662" TEXT="permanentes Vorhalten in der Ziel-Datenstruktur ist keine gute Idee"/>
<node CREATED="1456423940695" ID="ID_301596165" MODIFIED="1456423963088" TEXT="Platz wird ausschlie&#xdf;lich zur Diff-Anwendung gebraucht"/>
<node CREATED="1456423964572" ID="ID_1023557891" MODIFIED="1456423986445" TEXT="Stack-Struktur ist sinnvoll"/>
<node CREATED="1456424025228" ID="ID_1821511383" MODIFIED="1456424051133" TEXT="Kacheln, blockweise"/>
</node>
<node CREATED="1456428753110" FOLDED="true" ID="ID_900159058" MODIFIED="1488423308081" TEXT="Struktur">
<node CREATED="1456428763108" ID="ID_158540274" MODIFIED="1456428768063" TEXT="Frontend-Backend"/>
<node CREATED="1456428768579" ID="ID_1251866964" MODIFIED="1456428772654" TEXT="aber das Backend ist inline"/>
<node CREATED="1456428774171" ID="ID_745180197" MODIFIED="1456428896734" TEXT="Puffergr&#xf6;&#xdf;e initial festlegen"/>
<node CREATED="1456429302212" ID="ID_377071784" MODIFIED="1456429313666">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      rekursiv,
    </p>
    <p>
      duch Bindung bestimmt
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1456429330129" FOLDED="true" ID="ID_1923625809" MODIFIED="1488423308082" TEXT="technische Probleme">
<node CREATED="1456429337736" ID="ID_398416910" MODIFIED="1456429344154" TEXT="runtime - compiletime"/>
<node CREATED="1456429403455" ID="ID_166294730" MODIFIED="1456429408386" TEXT="Festlegen der Binde-Struktur"/>
<node CREATED="1456429465015" ID="ID_1980478069" MODIFIED="1456429476745" TEXT="&#xdc;bersetzen in eine Typ-Spezifikation"/>
<node CREATED="1456429488492" ID="ID_393439057" MODIFIED="1456429496150" TEXT="aus dem Typ eine Gr&#xf6;&#xdf;e ableiten"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1456430088803" FOLDED="true" HGAP="54" ID="ID_481518617" MODIFIED="1491009366955" TEXT="L&#xf6;sung" VSHIFT="19">
<icon BUILTIN="idea"/>
<node CREATED="1456430099658" FOLDED="true" ID="ID_820279076" MODIFIED="1488423342546" TEXT="Entscheidung">
<linktarget COLOR="#b10046" DESTINATION="ID_820279076" ENDARROW="Default" ENDINCLINATION="14;-46;" ID="Arrow_ID_103087571" SOURCE="ID_343687995" STARTARROW="None" STARTINCLINATION="-137;-20;"/>
<font NAME="SansSerif" SIZE="14"/>
<node CREATED="1456430153555" ID="ID_1609974475" MODIFIED="1456430170074" TEXT="es ist wesentlich"/>
<node CREATED="1456430115696" ID="ID_546849410" MODIFIED="1456430120227" TEXT="es ist notwendig"/>
<node CREATED="1456430109105" ID="ID_366046751" MODIFIED="1456430172153" TEXT="es ist machbar"/>
</node>
<node CREATED="1456430178935" FOLDED="true" HGAP="22" ID="ID_1069025314" MODIFIED="1488423342546" TEXT="der Weg..." VSHIFT="5">
<node CREATED="1456430183863" ID="ID_464613085" MODIFIED="1456430193433" TEXT="expliziter Stack"/>
<node CREATED="1456430194117" ID="ID_943734591" MODIFIED="1456430211847" TEXT="enth&#xe4;lt Front-End"/>
<node CREATED="1456430212811" ID="ID_90339221" MODIFIED="1456430233644" TEXT="und Backend in Puffer"/>
<node CREATED="1456430234656" ID="ID_1583269810" MODIFIED="1456430240155" TEXT="der Puffer ist eingebettet"/>
<node CREATED="1456430240887" FOLDED="true" ID="ID_1308625708" MODIFIED="1488423308112" TEXT="Grundmuster der Bindung">
<node CREATED="1456430374141" ID="ID_1551177855" MODIFIED="1456430398830" TEXT="Collection + Element-Konstruktur"/>
<node CREATED="1456430440196" ID="ID_1922438930" MODIFIED="1456430445551" TEXT="Attribut-Map als Getter/Setter"/>
<node CREATED="1456430515186" ID="ID_831818012" MODIFIED="1456430519166" TEXT="Rec&lt;GenNode&gt;"/>
</node>
<node CREATED="1457120439360" ID="ID_1972741688" MODIFIED="1487275490699" TEXT="ID match ist Aufgabe der Implementierung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das ist der wesentliche Kniff,
    </p>
    <p>
      durch den das Problem mit der &quot;absrakten, opaquen&quot; Position entsch&#228;rft wird
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456430363599" ID="ID_1608232847" MODIFIED="1456505525321" TEXT="erlaube typ-gefilterte Kinder"/>
</node>
</node>
<node COLOR="#338800" CREATED="1456506101544" FOLDED="true" HGAP="4" ID="ID_133511037" MODIFIED="1491009363371" TEXT="Implementierung" VSHIFT="16">
<icon BUILTIN="button_ok"/>
<node CREATED="1456506128581" FOLDED="true" HGAP="-12" ID="ID_322289358" MODIFIED="1488423342546" TEXT="Abw&#xe4;gungen" VSHIFT="184">
<node CREATED="1456506135028" FOLDED="true" HGAP="29" ID="ID_470489868" MODIFIED="1488423308113" TEXT="Indirektionen" VSHIFT="-5">
<node CREATED="1456506145826" ID="ID_759825167" MODIFIED="1461888854079" TEXT="kosten">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <ul>
      <li>
        Diff-Anwendung wird massiv und in der Breite stattfinden
      </li>
      <li>
        sie wird als Reaktion auf UI-Events auftreten
      </li>
      <li>
        sie dient dazu, andere UI-Operationen einzusparen
      </li>
      <li>
        also mu&#223; speziell das Traversieren bis an den Anwendungsort bedacht werden
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1456506151065" ID="ID_496007561" MODIFIED="1461888854085" TEXT="bisher nur drei">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...d.h. die bis jetzt geschriebene TreeApplikator-Implementierung
    </p>
    <p>
      ist erstaunlich leichtgewichtig. Zu den zwei Indirektionien der Sprache
    </p>
    <p>
      kommt nur entweder ein weiterer aus der GenNode bzw stattedessen ein dynamic cast hinzu.
    </p>
    <p>
      Alles andere steckt in dem <b>expliziten Mutator-Typ </b>
    </p>
    <p>
      &#160;-- das gibt einen wichtigen Hinweis --
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456506160472" ID="ID_1269579163" MODIFIED="1461888854089" TEXT="zwei sind das Minimum">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...da wir eine verb-basierte Sprache implementieren,
    </p>
    <p>
      also einen double-dispatch haben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456506385130" FOLDED="true" ID="ID_660502308" MODIFIED="1488423308098" TEXT="f&#xfc;r generische Zieldaten">
<node CREATED="1456506446002" ID="ID_485732185" MODIFIED="1456506470075" TEXT="kommt eine Indirektion hinzu"/>
<node CREATED="1456506471159" FOLDED="true" ID="ID_1228778249" MODIFIED="1488423308082" TEXT="es sei denn...">
<node CREATED="1456506475686" ID="ID_388141973" MODIFIED="1456506487232" TEXT="man tauscht jeweils den Sprach-Interpreter aus"/>
<node CREATED="1456506487796" ID="ID_24089490" MODIFIED="1456506499239" TEXT="das setzt aber rekursives Konsumieren des Diff voraus"/>
<node CREATED="1456506500187" ID="ID_438927454" MODIFIED="1456506511565" TEXT="und das zwingt uns, den Typ des Diff explizit zu machen"/>
<node CREATED="1456506512721" ID="ID_1007093555" MODIFIED="1461888854100" TEXT="der nicht-explizite Typ ist im Moment von Vorteil....">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      weil wir den Anwendungs-Kontext noch &#252;berhaupt nicht kennen.
    </p>
    <p>
      Man k&#246;nnte also sp&#228;ter, wenn das ganze System &quot;steht&quot;,
    </p>
    <p>
      das Diff-System noch einmal reimplementieren, dann mit einem vorgegebenen Diff-Typ
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1456506607428" HGAP="31" ID="ID_448113584" MODIFIED="1456506627511" VSHIFT="8">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Beschlu&#223;: <i>akzeptiert</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1456506647447" FOLDED="true" HGAP="32" ID="ID_399090389" MODIFIED="1488423308098" TEXT="Zielvorgabe: weitere vermeiden" VSHIFT="12">
<node CREATED="1456506661421" ID="ID_593167689" MODIFIED="1456506689357" TEXT="d.h. nur entweder front-end oder back-end sind virtuell"/>
<node CREATED="1456506689834" ID="ID_1393928517" MODIFIED="1456506713270">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      im Sinn von &quot;polymorpic value&quot; ist <b>das Backend virtuell</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1456506719949" ID="ID_326439598" MODIFIED="1456506725568" TEXT="keine virtuellen Iteratoren"/>
<node CREATED="1456506726060" ID="ID_999652076" MODIFIED="1456506738455" TEXT="Front-End mu&#xdf; Typedefs bereitstellen"/>
<node CREATED="1456506740323" ID="ID_1961807979" MODIFIED="1456506747838" TEXT="elaboriertes Typkonstrukt notwendig"/>
<node CREATED="1456506749234" ID="ID_957942327" MODIFIED="1461888854111" TEXT="zumindest f&#xfc;r jeden Compund-Zieltyp ein eigener Interpreter">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....wenngleich auch dieser aus einem Template generiert wird
    </p>
    <p>
      (will sagen, es ist nicht sofort offensichtlich, da&#223; wir jeweils einen Interpreter generieren)
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1457038737878" FOLDED="true" HGAP="44" ID="ID_643190842" MODIFIED="1488423308113" TEXT="Abstraktions-Grad">
<node CREATED="1457119812475" ID="ID_1862440484" MODIFIED="1457119817575" TEXT="Elemente bleiben opaque"/>
<node CREATED="1457119818091" FOLDED="true" ID="ID_1097779694" MODIFIED="1488423308098">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wir <i>verzichten</i>&#160;auf Introspektion der Elemente
    </p>
  </body>
</html></richcontent>
<node CREATED="1457119990420" ID="ID_1521035606" MODIFIED="1457120011276" TEXT="...denn die m&#xfc;&#xdf;te die jeweilige Implementierung erbringen"/>
<node CREATED="1457120011721" ID="ID_23476594" MODIFIED="1457120024027" TEXT="und das w&#xfc;rde das &#xdc;bel der Introspektion &#xfc;berall hineindr&#xfc;cken"/>
<node CREATED="1457120026343" ID="ID_156752160" MODIFIED="1457120043288" TEXT="Nachteil: keine klare Fehlerdiagnose m&#xf6;glich"/>
<node CREATED="1457120043925" FOLDED="true" ID="ID_1878678132" MODIFIED="1488423308082" TEXT="ist akzeptierbar...">
<node CREATED="1457120079542" ID="ID_1809509099" MODIFIED="1457120079542">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denn genau zu diesem Zweck haben wir die &quot;External Tree Description&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1457120084135" ID="ID_633070963" MODIFIED="1457120093706" TEXT="auf diese sollte ein Diff zuerst angewendet werden"/>
<node CREATED="1457120094302" ID="ID_1409151619" MODIFIED="1457120105488" TEXT="und diese bietet Introspektion und klare Diagnose"/>
<node CREATED="1457120105908" ID="ID_1245554045" MODIFIED="1457120131741" TEXT="Annahme ist also, da&#xdf; ein Diff auf real-Datenstrukturen per Konstruktion erfolgreich ist"/>
<node CREATED="1457120134297" ID="ID_1098514912" MODIFIED="1457120154250" TEXT="und ein Fehler dabei ist zwingenderweise ein Fehler in der Programmlogik"/>
</node>
</node>
<node CREATED="1457119844767" ID="ID_1648334868" MODIFIED="1457119861265" TEXT="wir bieten nur einen Match auf die Diff-Spec"/>
<node CREATED="1457119876899" ID="ID_362462955" MODIFIED="1457119902755" TEXT="Mutations-Primitive bieten bool Erfolgs-Check"/>
<node CREATED="1457119903391" ID="ID_1244810870" MODIFIED="1457119909602" TEXT="aber keine Exceptions"/>
</node>
</node>
<node CREATED="1456506821808" FOLDED="true" ID="ID_1852134358" MODIFIED="1488423342547" TEXT="Typ-Konstrukt">
<node CREATED="1461879209050" ID="ID_983143410" MODIFIED="1461879267131" TEXT="Kernproblem: wie gro&#xdf; mu&#xdf; der Buffer sein?">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1461879239310" ID="ID_866573163" MODIFIED="1461879260051" TEXT="wie bekomme ich diese Info von nicht a priori bekannten Typen">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1456523437616" FOLDED="true" HGAP="34" ID="ID_410606899" MODIFIED="1488423342547" TEXT="Manipulations-Interface" VSHIFT="6">
<node COLOR="#338800" CREATED="1456523455997" FOLDED="true" ID="ID_1416114013" MODIFIED="1488423308113" TEXT="Design l&#xf6;sen">
<icon BUILTIN="button_ok"/>
<node CREATED="1456523507910" ID="ID_429333479" MODIFIED="1456523518005" TEXT="Henne oder Ei?"/>
<node CREATED="1456523471851" ID="ID_1273540009" MODIFIED="1456523485597" TEXT="komm nicht vom Fleck"/>
<node CREATED="1456523487089" ID="ID_1139978684" MODIFIED="1456523501995" TEXT="zirkul&#xe4;re Bez&#xfc;ge im Design"/>
<node CREATED="1456523522693" FOLDED="true" ID="ID_625051251" MODIFIED="1488423308098" TEXT="abstrakter Entwurf">
<arrowlink COLOR="#6b77a6" DESTINATION="ID_728198359" ENDARROW="Default" ENDINCLINATION="-116;0;" ID="Arrow_ID_340427685" STARTARROW="None" STARTINCLINATION="-819;487;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1456523558304" ID="ID_1843819253" MODIFIED="1456523564050" TEXT="baue einen Dummy">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1456523569111" ID="ID_1173914226" MODIFIED="1456523580721" TEXT="...als eine Spezialisierung des Adapters"/>
<node CREATED="1456523593731" ID="ID_126624039" MODIFIED="1456528188767" TEXT="Interface ohne reale Daten"/>
<node CREATED="1456528076005" FOLDED="true" ID="ID_1973839094" MODIFIED="1488423308082" TEXT="Diagnose">
<node CREATED="1456528119297" ID="ID_37448528" MODIFIED="1456528119297" TEXT="hat ein EventLog eingebaut"/>
<node CREATED="1456528148107" ID="ID_626507029" MODIFIED="1456528165925" TEXT="protokolliert Schatten-Kinder-Liste"/>
<node CREATED="1456528167440" ID="ID_1984138384" MODIFIED="1472219338598" TEXT="transparenter Dekorator">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...d.h,
    </p>
    <p>
      kann zus&#228;tzlich zu einem anderen Adaptor
    </p>
    <p>
      in die Mutator-Dekorator-Kette geh&#228;ngt werden
    </p>
    <p>
      und protokolliert somit &quot;nebenbei&quot; was an Anforderungen an ihm vorbeigeht
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1456523612849" ID="ID_729852941" MODIFIED="1456523625883" TEXT="Test zum Aufbau des Interfaces"/>
</node>
</node>
<node CREATED="1456528418823" FOLDED="true" HGAP="30" ID="ID_1372068793" MODIFIED="1488423308113" TEXT="Bestandteile" VSHIFT="-1">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1456528424246" FOLDED="true" ID="ID_472044094" MODIFIED="1488423308098" STYLE="fork" TEXT="Selektor">
<linktarget COLOR="#7caed5" DESTINATION="ID_472044094" ENDARROW="Default" ENDINCLINATION="466;133;" ID="Arrow_ID_1253352795" SOURCE="ID_1406395599" STARTARROW="None" STARTINCLINATION="681;0;"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1457741609884" FOLDED="true" ID="ID_1827838196" MODIFIED="1488423308082" TEXT="bin ich zust&#xe4;ndig?">
<node CREATED="1458094312266" ID="ID_233521483" MODIFIED="1458094316157" TEXT="isApplicable"/>
<node CREATED="1458094316697" FOLDED="true" ID="ID_839136400" MODIFIED="1488423308059" TEXT="Festlegung:">
<icon BUILTIN="yes"/>
<node CREATED="1458094327095" ID="ID_306307671" MODIFIED="1458094334354" TEXT="nur auf Basis der spec"/>
<node CREATED="1458094334991" ID="ID_903070750" MODIFIED="1458094343401" TEXT="ohne Daten-Introspektion"/>
</node>
<node CREATED="1458094507048" FOLDED="true" ID="ID_1012976053" MODIFIED="1488423308059" TEXT="Problem">
<node CREATED="1458094510463" FOLDED="true" ID="ID_812427058" MODIFIED="1488423307001" TEXT="Spec bietet wenig Anhaltspunkte">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      streng genommen ist es nur erlaubt, das ID-Symbol auszuwerten
    </p>
  </body>
</html></richcontent>
<node CREATED="1458094562800" ID="ID_184244927" MODIFIED="1458094568467" TEXT="ID-Symbol"/>
<node CREATED="1458094569128" ID="ID_743935773" MODIFIED="1458094579369" TEXT="attribute / child"/>
</node>
<node CREATED="1458094599835" FOLDED="true" HGAP="24" ID="ID_1161622136" MODIFIED="1488423307001" TEXT="Zugang zum Payload-Typ" VSHIFT="10">
<icon BUILTIN="help"/>
<node CREATED="1458094616417" ID="ID_1013344827" MODIFIED="1458094632012" TEXT="Introspection / Switch-on-Type">
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1458094634167" ID="ID_1794163270" MODIFIED="1458094672710">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Visitor bedeutet <b>zwei</b>&#160;Indirektionen
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und das ist nicht akzeptabel f&#252;r ein reines Selektor-Pr&#228;dikat!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458094716364" FOLDED="true" ID="ID_1749838430" MODIFIED="1488423307001" TEXT="eingeschr&#xe4;nkt auf Objekt-Typ">
<icon BUILTIN="help"/>
<node CREATED="1458094735673" ID="ID_1089998465" MODIFIED="1458094743492" TEXT="Semantischer Typ"/>
<node CREATED="1458094743960" ID="ID_1642238646" MODIFIED="1458094767704" TEXT="vs. Typisierung in der Sprache"/>
<node CREATED="1458094769365" ID="ID_855557799" MODIFIED="1458094773848" TEXT="pattern match"/>
<node CREATED="1458094823973" ID="ID_726480550" MODIFIED="1458094841971">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denkbar nur bei Sub-<i>Objekten</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458094850490" ID="ID_39407126" MODIFIED="1458094856797" TEXT="d.h. bei Record-Daten"/>
<node CREATED="1458094864424" ID="ID_746601299" MODIFIED="1458094872787" TEXT="Record hat ein (semantisches) Typfeld"/>
<node CREATED="1458094922024" ID="ID_1616971769" MODIFIED="1458094954604">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gilt f&#252;r <i>alle</i>&#160;praktischen Anwendungen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458094956636" ID="ID_1820565901" MODIFIED="1458095165033" TEXT="denn nur Objekt-Kinder machen Sinn">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....auch wenn man zehnmal meinen k&#246;nnte,
    </p>
    <p>
      Kinder eines reinen Wert-Typs w&#228;ren sinnvoll --
    </p>
    <p>
      sie sind es nicht!
    </p>
    <p>
      Jede sinnvolle <i>Entit&#228;t</i>&#160;hat mehr als ein Attribut<i>! </i>
    </p>
    <p>
      denn es macht keinen Sinn, Entit&#228;ten und reine Wert-Elemente
    </p>
    <p>
      auf der gleichen Ebene in der gleichen Sammlung zu mischen.
    </p>
    <p>
      
    </p>
    <p>
      D.h., entweder man hat ein Objekt, das als Kinder z.B. eine Liste von Strings hat,
    </p>
    <p>
      oder man hat eine Entit&#228;t, die z.b. zwei getypte Objekt-Kinder-Sammlungen hat,
    </p>
    <p>
      wie z.B: eine Spur mit Labels und Clips
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458095217737" FOLDED="true" ID="ID_824115387" MODIFIED="1488423307001" TEXT="um das zu unterst&#xfc;tzen...">
<icon BUILTIN="idea"/>
<node CREATED="1458095230175" ID="ID_995706334" MODIFIED="1458095239258" TEXT="m&#xfc;&#xdf;te DataCap helfen"/>
<node CREATED="1458095367701" ID="ID_1549321280" MODIFIED="1458095376400" TEXT="es g&#xe4;be dann einen Bottom-Wert"/>
<node CREATED="1458095240158" ID="ID_1884817792" MODIFIED="1458095284084" TEXT="Introspektions-Verbot etwas aufgeweicht"/>
<node CREATED="1458095304886" ID="ID_395248655" MODIFIED="1458095318800" TEXT="dieser Gebrauch ist aber mit dem Objekt-Typfeld schon angelegt"/>
<node CREATED="1458095320692" ID="ID_1203007776" MODIFIED="1458095332942" TEXT="die Bedeutung wird vom Client privat definiert"/>
<node CREATED="1458095409800" ID="ID_227738227" MODIFIED="1458095442503" TEXT="ebenso wird aber auch die Bedeutung der ETD lokal definiert"/>
<node CREATED="1458095443115" ID="ID_586715428" MODIFIED="1458095455459" TEXT="erscheint mir plausibel und vertretbar">
<icon BUILTIN="yes"/>
</node>
<node COLOR="#338800" CREATED="1461881999377" ID="ID_1469132046" MODIFIED="1461882004217" TEXT="so implementiert">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1458095528224" FOLDED="true" ID="ID_109058224" MODIFIED="1488423308060" TEXT="falls nein...">
<node CREATED="1458095540807" ID="ID_225712554" MODIFIED="1458095553929" TEXT="&#xfc;bernimmt der n&#xe4;chste Onion-Layer"/>
<node CREATED="1458095554548" ID="ID_1745271000" MODIFIED="1458095566303" TEXT="scheitert am Ende die Diff-Anwendung"/>
<node CREATED="1458095567123" FOLDED="true" ID="ID_1135449086" MODIFIED="1488423307001" TEXT="das kann nicht sein">
<icon BUILTIN="ksmiletris"/>
<node CREATED="1458095599015" ID="ID_1548761670" MODIFIED="1458095611129" TEXT="denn Struktur wird per Diff aufgebaut"/>
<node CREATED="1458095611645" ID="ID_1917053068" MODIFIED="1458095623023" TEXT="und Diff wird von einer konsistenten Struktur generiert"/>
<node CREATED="1458095623563" ID="ID_1563236319" MODIFIED="1458095636789" TEXT="wenngleich es sich auch um eine funktionale Struktur handeln kann..."/>
<node CREATED="1458095660150" ID="ID_329824783" MODIFIED="1458095668777" TEXT="wir sind nicht tolerant bei der Diff-Anwendung"/>
<node CREATED="1458095669437" ID="ID_650500191" MODIFIED="1458095672801" TEXT="und das ist gut so."/>
</node>
</node>
</node>
<node CREATED="1457741617655" FOLDED="true" ID="ID_1664250793" MODIFIED="1488423308082" TEXT="was ist ein &quot;match&quot;?">
<node CREATED="1458094392679" ID="ID_238187527" MODIFIED="1460755241888">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &quot;target <i>matches</i>&#160;spec&quot;
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1458094428506" ID="ID_1753032814" MODIFIED="1458094435693" TEXT="darf Annahmen &#xfc;ber den Typ der Spec machen"/>
<node CREATED="1458094397350" ID="ID_1185807177" MODIFIED="1458094427326" TEXT="darf in die Payload der Spec schauen"/>
<node CREATED="1458094437249" ID="ID_1125952135" MODIFIED="1458094463529" TEXT="darf Methoden des Datenobjekts aufrufen"/>
</node>
</node>
<node CREATED="1457038643034" FOLDED="true" ID="ID_1057400532" MODIFIED="1488423308098" TEXT="abstrakte Position">
<node CREATED="1460755065016" ID="ID_935271147" MODIFIED="1460755138704" TEXT="wird niemals direkt repr&#xe4;sentiert">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1460755072903" ID="ID_1650064575" MODIFIED="1460755130181">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aber existiert <i>nominell </i>und kontext-abh&#228;ngig
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1460755080894" ID="ID_107894920" MODIFIED="1460755106150" TEXT="und faktisch unabh&#xe4;ngig in jedem &#xbb;onion layer&#xab;"/>
</node>
<node CREATED="1457038527402" FOLDED="true" HGAP="16" ID="ID_1909805079" MODIFIED="1488423308098" TEXT="Mutations-Primitive" VSHIFT="7">
<node CREATED="1457038788583" ID="ID_131880970" MODIFIED="1457038801193" TEXT="einheitlich benannt"/>
<node CREATED="1457038801733" ID="ID_428479460" MODIFIED="1457038805816" TEXT="auf mehreren Ebenen"/>
<node CREATED="1457119943698" FOLDED="true" ID="ID_370768211" MODIFIED="1488423308082" TEXT="Fehlerbehandlung">
<node CREATED="1457119949785" ID="ID_658419110" MODIFIED="1457119953012" TEXT="Match-Check"/>
<node CREATED="1457119953384" ID="ID_1688941824" MODIFIED="1457119957324" TEXT="aber keine Exception"/>
<node CREATED="1457119959408" ID="ID_275413703" MODIFIED="1457119961803" TEXT="sondern NOP"/>
</node>
</node>
<node CREATED="1457227228502" FOLDED="true" ID="ID_845299775" MODIFIED="1488423308099" TEXT="sub-Mutator">
<node CREATED="1457227237269" ID="ID_1175871622" MODIFIED="1457227296340" TEXT="wird &#xfc;ber ein Handle implantiert">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1457227248779" ID="ID_1765753442" MODIFIED="1457227279155" TEXT="Implementierungs-Schicht kennt Typ"/>
<node CREATED="1457227279703" ID="ID_347493370" MODIFIED="1457227288610" TEXT="aber Tree-Mutator &#xfc;bernimmt ownership"/>
</node>
</node>
</node>
<node CREATED="1456528462585" FOLDED="true" HGAP="48" ID="ID_1770521063" MODIFIED="1488423342547" TEXT="Elemente" VSHIFT="21">
<node CREATED="1456528472016" ID="ID_315938795" MODIFIED="1456528476211" TEXT="bleiben abstrakt"/>
<node CREATED="1456528477415" FOLDED="true" ID="ID_1931717091" MODIFIED="1488423308113" TEXT="abstrakte &quot;Position&quot;">
<node CREATED="1456533135345" ID="ID_1775378899" MODIFIED="1465428629652" TEXT="Problem: wem geh&#xf6;rt diese Position?">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1456533146055" ID="ID_312959811" MODIFIED="1456533213989" TEXT="mu&#xdf; stets &#xfc;ber API klar gemacht werden">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1456533166197" ID="ID_1000253494" MODIFIED="1456533204589" TEXT="der Selektor entscheidet, wer gemeint ist">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1456528485326" FOLDED="true" ID="ID_1764970284" MODIFIED="1488423308113" TEXT="getypte Kinder">
<node CREATED="1456528491109" ID="ID_1550088866" MODIFIED="1456528504567" TEXT="auf Einzelfall-Basis"/>
<node CREATED="1456528505395" ID="ID_1091163328" MODIFIED="1456528519117" TEXT="wer per Selektor zust&#xe4;ndig ist"/>
<node CREATED="1456528539799" ID="ID_1658968937" MODIFIED="1456528550465" TEXT="Typisierung ergibt sich als Effekt"/>
<node CREATED="1456528551029" ID="ID_193586898" MODIFIED="1465428643785" TEXT="Typisierung wird nicht eigens repr&#xe4;sentiert">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1457232376244" HGAP="29" ID="ID_1914581822" MODIFIED="1491009351080" TEXT="onion layer" VSHIFT="26">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das sind die konkreten Implementierungen
    </p>
    <p>
      f&#252;r spezifische Arten von Bindings
    </p>
  </body>
</html></richcontent>
<node CREATED="1457232408368" FOLDED="true" HGAP="29" ID="ID_1068782263" MODIFIED="1491009360796" TEXT="TestWireTap / TestMutationTarget" VSHIFT="-6">
<icon BUILTIN="full-1"/>
<node CREATED="1457232426238" ID="ID_728198359" MODIFIED="1457232567024" TEXT="brauche ich, um das API zu entwickeln">
<linktarget COLOR="#6b77a6" DESTINATION="ID_728198359" ENDARROW="Default" ENDINCLINATION="-116;0;" ID="Arrow_ID_340427685" SOURCE="ID_625051251" STARTARROW="None" STARTINCLINATION="-819;487;"/>
</node>
<node CREATED="1457232433157" ID="ID_1606470322" MODIFIED="1457232443415" TEXT="ggfs sp&#xe4;ter n&#xfc;tzlich zur Diagnose"/>
<node CREATED="1457232444075" ID="ID_442780922" MODIFIED="1457232459765" TEXT="soll stets &#xfc;ber andere, konkrete Mutation-Layer gelegt werden"/>
<node CREATED="1457232579073" FOLDED="true" ID="ID_280491578" MODIFIED="1488423308099" TEXT="Grenzen">
<node CREATED="1457232582704" ID="ID_1594229064" MODIFIED="1457232590595" TEXT="protokolliert nur String-Repr&#xe4;sentation"/>
<node CREATED="1457232591087" ID="ID_119032593" MODIFIED="1472219338677">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann <i>niemals </i>geschachtelte sub-Mutatoren modellieren
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ja wirklich, das w&#228;re nicht sinnvoll!!!!!
    </p>
    <p>
      auch wenn man meinen k&#246;nnte, es geht.
    </p>
    <p>
      Grund ist n&#228;mlich, es kann jeweils nur ein Onion-Layer f&#252;r ein gegebenes Element &quot;zust&#228;ndig&quot; sein.
    </p>
    <p>
      Und aus Gr&#252;nden der logischen Konsistenz darf dieser Diagnose-Layer niemals f&#252;r ein Element zust&#228;ndig sein,
    </p>
    <p>
      denn sonst w&#252;rde er es f&#252;r darunter liegende Layer verschatten.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="closed"/>
</node>
</node>
</node>
<node CREATED="1457232734636" FOLDED="true" ID="ID_1873945047" MODIFIED="1491009359164" TEXT="STL Collection">
<icon BUILTIN="full-2"/>
<node CREATED="1458175823858" FOLDED="true" ID="ID_438044714" MODIFIED="1488423308099" TEXT="Design-Grundlagen">
<node CREATED="1457741673001" FOLDED="true" ID="ID_1873415490" MODIFIED="1488423308082" TEXT="konkrete Beispiele">
<icon BUILTIN="info"/>
<node CREATED="1457741683687" ID="ID_1222762304" MODIFIED="1457741691409" TEXT="Timelines"/>
<node CREATED="1457741691837" ID="ID_1487568890" MODIFIED="1457741697832" TEXT="Tracks"/>
<node CREATED="1457741753006" ID="ID_303387812" MODIFIED="1457742102003" TEXT="Clips / Effekte / Marken"/>
<node CREATED="1457741809422" ID="ID_1428407098" MODIFIED="1457741815657" TEXT="Effekte / Transitions"/>
</node>
<node CREATED="1457742434714" ID="ID_587627505" MODIFIED="1457742717208">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &#10233; <i>immer</i>&#160;Mitwirkung des Elements
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1457742547067" FOLDED="true" HGAP="33" ID="ID_410938096" MODIFIED="1488423308082" TEXT="Selector explizit bauen" VSHIFT="7">
<icon BUILTIN="yes"/>
<node CREATED="1457742554722" ID="ID_1406395599" MODIFIED="1469918265250" TEXT="Selector-Interface">
<arrowlink COLOR="#7caed5" DESTINATION="ID_472044094" ENDARROW="Default" ENDINCLINATION="466;133;" ID="Arrow_ID_1253352795" STARTARROW="None" STARTINCLINATION="681;0;"/>
</node>
<node CREATED="1457742626368" FOLDED="true" ID="ID_1910692241" MODIFIED="1488423308060" TEXT="2.Ausdrucks-Ebene">
<node CREATED="1457742639703" ID="ID_1065867780" MODIFIED="1457742645826" TEXT="neben der Identit&#xe4;t"/>
<node CREATED="1457742646366" FOLDED="true" ID="ID_1533568989" MODIFIED="1488423307049" TEXT="logische Typisierung">
<node CREATED="1457742667947" ID="ID_1640657752" MODIFIED="1457742677734" TEXT="entweder anhand Payload-Typ"/>
<node CREATED="1457742678185" ID="ID_1211029199" MODIFIED="1457742684533" TEXT="oder versteckt in der ID"/>
</node>
</node>
</node>
</node>
<node CREATED="1458175861176" FOLDED="true" HGAP="33" ID="ID_44294405" MODIFIED="1488423308099" TEXT="Design-Anforderungen">
<node CREATED="1458175880034" FOLDED="true" ID="ID_995745627" MODIFIED="1488423308083" TEXT="gegeben ist eine STL-Collection">
<node CREATED="1458175892400" ID="ID_1308086929" MODIFIED="1458175904674" TEXT="allgemeinstes STL-Container-Interface"/>
<node CREATED="1458175905766" ID="ID_446727516" MODIFIED="1458175919817" TEXT="Wert-Typ per typedef abgreifen"/>
</node>
<node CREATED="1455927276314" FOLDED="true" HGAP="91" ID="ID_1959700641" MODIFIED="1488423308083" TEXT="brauche zus&#xe4;tzliche Operationen" VSHIFT="-3">
<cloud COLOR="#fce9c0"/>
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1458175942866" FOLDED="true" ID="ID_181058597" MODIFIED="1488423308060" TEXT="match">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177113846" ID="ID_320488998" MODIFIED="1458177953094" TEXT="stellt fest, ob das Ziel pa&#xdf;t"/>
<node CREATED="1458177136475" ID="ID_1352800435" MODIFIED="1458177953094" TEXT="greift auf eine ID des Zieles zur&#xfc;ck"/>
<node CREATED="1458177147897" ID="ID_1632332105" MODIFIED="1458177953094" TEXT="mu&#xdf; daher in der Regel bereitgestellt werden"/>
<node CREATED="1458177166543" FOLDED="true" ID="ID_1266232529" MODIFIED="1488423307062" TEXT="Ersatz">
<icon BUILTIN="idea"/>
<node CREATED="1458177169406" ID="ID_1194870730" MODIFIED="1458177953094" TEXT="content-Match"/>
<node CREATED="1458177173574" ID="ID_1636541063" MODIFIED="1458177953094" TEXT="Konsequenz: man kann keine Assignements machen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      weil beim Assignment die Spec (=GenNode) eben
    </p>
    <p>
      zwar die ID des Zieles, aber den neu zuzuweisenden Wert enth&#228;lt.
    </p>
    <p>
      Also wird sich das Ziel nicht anhand des neuen Wertes finden lassen,
    </p>
    <p>
      weil es eben grade noch nicht diesen neuen Wert tr&#228;gt.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458177198643" ID="ID_1228381833" MODIFIED="1458177953094" TEXT="Konsequenz: Suche kann das falsche Resultat erwischen"/>
</node>
</node>
<node CREATED="1458175960615" FOLDED="true" ID="ID_499742122" MODIFIED="1488423308060" TEXT="ctor">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177315803" ID="ID_1282762906" MODIFIED="1458177953094" TEXT="konstruiert aus der Spec ein neues Element"/>
<node CREATED="1458177335632" ID="ID_430010657" MODIFIED="1458177953094" TEXT="mu&#xdf; typischerweise die Interna des Zieles kennen"/>
<node CREATED="1458177347622" FOLDED="true" ID="ID_1041373197" MODIFIED="1488423307062" TEXT="Ersatz">
<icon BUILTIN="idea"/>
<node CREATED="1458177489444" ID="ID_1024908643" MODIFIED="1458177953094" TEXT="unterstelle Payload == Zieltyp"/>
<node CREATED="1458177572865" ID="ID_1483034601" MODIFIED="1458177953094" TEXT="Konstruktor auf dem Ziel, der eine GenNode nimmt"/>
</node>
</node>
<node CREATED="1458175991795" FOLDED="true" ID="ID_437650849" MODIFIED="1488423308060" TEXT="optional...">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458176002762" FOLDED="true" ID="ID_442473703" MODIFIED="1488423307063" TEXT="selector">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177605869" ID="ID_262186866" MODIFIED="1458177953094" TEXT="bestimmt ob die Spec f&#xfc;r diesen Binding-Layer &#xfc;berhaupt relevant ist"/>
<node CREATED="1458177631409" ID="ID_1877742460" MODIFIED="1458177953094" TEXT="erlaubt das Filtern mit mehreren Bindings"/>
<node CREATED="1458177642144" FOLDED="true" ID="ID_310751196" MODIFIED="1488423307062" TEXT="Ersatz">
<icon BUILTIN="idea"/>
<node CREATED="1458177646671" ID="ID_1707674287" MODIFIED="1458177953094" TEXT="kein Selector"/>
<node CREATED="1458177652662" ID="ID_1067744704" MODIFIED="1458177953094" TEXT="es kann dann also nur ein Binding geben"/>
<node CREATED="1458177664868" ID="ID_935075867" MODIFIED="1458177953094" TEXT="das ist aber auch der h&#xe4;ufigste Fall"/>
</node>
</node>
<node CREATED="1458176008025" FOLDED="true" ID="ID_1099350141" MODIFIED="1488423307063" TEXT="setter">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177697657" ID="ID_1388855448" MODIFIED="1458177953095" TEXT="erlaubt Zuweisung"/>
<node CREATED="1458177712798" ID="ID_1708999099" MODIFIED="1458177953095" TEXT="setzt Auswahl per match voraus"/>
<node CREATED="1458177735635" FOLDED="true" ID="ID_197038069" MODIFIED="1488423307063" TEXT="Ersatz">
<icon BUILTIN="button_cancel"/>
<node CREATED="1458177738499" ID="ID_1474971428" MODIFIED="1458177953095" TEXT="keiner"/>
<node CREATED="1458177741155" ID="ID_1784046352" MODIFIED="1458177953095" TEXT="dann gibt&apos;s eben keine Zuweisung"/>
<node CREATED="1458177749233" ID="ID_873905001" MODIFIED="1458177953095" TEXT="auch nicht n&#xf6;tig bei sub-Scopes"/>
<node CREATED="1458177803730" ID="ID_674080114" MODIFIED="1458177953095" TEXT="auch nicht n&#xf6;tig bei immutable values"/>
</node>
</node>
<node CREATED="1458176018087" FOLDED="true" ID="ID_1978664300" MODIFIED="1488423307063" TEXT="mutator">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177821712" ID="ID_152424824" MODIFIED="1458177953095" TEXT="baut einen sub-Mutator f&#xfc;r einen nested scope"/>
<node CREATED="1458177851988" ID="ID_307170712" MODIFIED="1458177953095" TEXT="setzt Auswahl per match voraus"/>
<node CREATED="1458177857939" FOLDED="true" ID="ID_233644864" MODIFIED="1488423307063" TEXT="Ersatz">
<icon BUILTIN="button_cancel"/>
<node CREATED="1458177861162" ID="ID_240936470" MODIFIED="1458177953095" TEXT="keiner"/>
<node CREATED="1458177863506" ID="ID_1403138553" MODIFIED="1458177953095" TEXT="dann gibts eben keine Behandlung von nested scopse"/>
<node CREATED="1458177874760" ID="ID_1050027667" MODIFIED="1458177953095" TEXT="auch nicht n&#xf6;tig bei primitive values"/>
</node>
</node>
</node>
</node>
<node CREATED="1458176048564" FOLDED="true" ID="ID_173428941" MODIFIED="1488423308083" TEXT="Flexibilit&#xe4;t">
<node CREATED="1458176054154" ID="ID_1379937028" MODIFIED="1458176076851" TEXT="so wenig typ-Feslegung wie m&#xf6;glich"/>
<node CREATED="1458176077368" ID="ID_1730540256" MODIFIED="1458176092658" TEXT="fallback bei fehlendern Operationen"/>
<node CREATED="1458176093958" FOLDED="true" ID="ID_801042454" MODIFIED="1488423308060" TEXT="Argument: &quot;dann halt nicht&quot;">
<node CREATED="1458176117266" ID="ID_1479654896" MODIFIED="1458176127813" TEXT="es ist so wie es ist"/>
<node CREATED="1458176128352" ID="ID_1366947640" MODIFIED="1458176139563" TEXT="die Gegenseite wei&#xdf; schon was sie will"/>
<node CREATED="1458176252336" FOLDED="true" ID="ID_1554344013" MODIFIED="1488423307063" TEXT="ich bin, also bin ich konsistent">
<node CREATED="1458176267382" ID="ID_427126071" MODIFIED="1458176296319" TEXT="konsistent ist, was der diff erzeugt"/>
<node CREATED="1458176297282" ID="ID_336107495" MODIFIED="1458176305981" TEXT="alles wird per diff erzeugt"/>
<node CREATED="1458176306593" ID="ID_666376142" MODIFIED="1458176314868" TEXT="also ist alles, was existiert, auch konsistent"/>
</node>
<node CREATED="1458176787953" FOLDED="true" ID="ID_1610561572" MODIFIED="1488423307064" TEXT="sinnvoll gew&#xe4;hlt">
<node CREATED="1458176794848" ID="ID_1399218405" MODIFIED="1458176810370" TEXT="native datenstruktur ist so gew&#xe4;hlt, da&#xdf; sie ihren Zweck erf&#xfc;llt"/>
<node CREATED="1458176883836" ID="ID_1357445271" MODIFIED="1458176899324">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      generische Repr&#228;sentaton ist so gew&#228;hlt,
    </p>
    <p>
      da&#223; sich alle relevanten Eigenschaften darstellen lassen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458176911161" FOLDED="true" ID="ID_1565618197" MODIFIED="1488423307063">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn also ein Teil der diff-Funkttionalit&#228;t nicht verf&#252;gbar ist,
    </p>
    <p>
      dann wird es wohl so sein, da&#223; sie auch nicht gebraucht wird
    </p>
  </body>
</html></richcontent>
<node CREATED="1458176947972" ID="ID_1901451299" MODIFIED="1458176965869" TEXT="Beispiel: primitive Werte sind eben kein sub-Scope"/>
<node CREATED="1458176966665" ID="ID_1292201640" MODIFIED="1458176981459" TEXT="Beispiel: immutable elements sind eben nicht zuweisbar"/>
</node>
</node>
</node>
<node CREATED="1458176998437" ID="ID_425451497" MODIFIED="1460752973198" TEXT="keine unn&#xf6;tigen Einschr&#xe4;nkungen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      zwar erscheint es nicht sonderlich sinnvoll,
    </p>
    <p>
      als target auch eine Menge von primitiven Werten zuzulassen.
    </p>
    <p>
      
    </p>
    <p>
      Es gibt aber auch keinen wirklichen Grund, dies zu verbieten,
    </p>
    <p>
      sofern es gelingt, die Funktionalit&#228;t gutm&#252;tig zu degradieren.
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1458178113697" FOLDED="true" HGAP="46" ID="ID_186668887" MODIFIED="1488423308099" TEXT="Design-Schlu&#xdf;folgerungen" VSHIFT="-5">
<node CREATED="1458178126159" ID="ID_1322433318" MODIFIED="1458178131587" TEXT="Rahmenklasse + Closures"/>
<node CREATED="1458178133078" ID="ID_278002579" MODIFIED="1458851636692" TEXT="Closures mit der Ersatz-Impl vorbelegen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1458178150916" ID="ID_1020570949" MODIFIED="1458178174848" TEXT="Problem: komplexe Konstruktion">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1458178165322" FOLDED="true" ID="ID_963054881" MODIFIED="1488423308083" TEXT="L&#xf6;sung: nested DSL">
<icon BUILTIN="idea"/>
<node CREATED="1458178203821" ID="ID_785912532" MODIFIED="1458178222190" TEXT="die Top-Level Builder-Funktion erwartet das volle Binding"/>
<node CREATED="1458178223723" ID="ID_798018180" MODIFIED="1458178247755" TEXT="es gibt eine freie Funktion, die einen nested Builder f&#xfc;r das Binding liefert"/>
</node>
</node>
<node COLOR="#338800" CREATED="1458178288690" FOLDED="true" ID="ID_562189277" MODIFIED="1488423308099" TEXT="Implementierung">
<icon BUILTIN="button_ok"/>
<node CREATED="1458178298328" FOLDED="true" ID="ID_1964300614" MODIFIED="1488423308083" TEXT="Builder">
<node CREATED="1458325962991" ID="ID_1955400674" MODIFIED="1460753004725" TEXT="Typ-Definitionen schwierig anzuschreiben">
<icon BUILTIN="smiley-neutral"/>
</node>
<node CREATED="1458325973486" ID="ID_1711903796" MODIFIED="1458325992405" TEXT="Code ziemlich redundant und undurchsichtig">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1458326045724" ID="ID_338749301" MODIFIED="1472498604875" TEXT="Typ-Check erst sehr sp&#xe4;t">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...will sagen,
    </p>
    <p>
      da sind mehrere Layer an praktisch ungebundenem Template-Code dazwischen,
    </p>
    <p>
      so da&#223; zu bef&#252;rchten steht, da&#223; ein unpassendes Lambda erst weit entfernt
    </p>
    <p>
      eine wom&#246;glich irref&#252;hrende Meldung generiert
    </p>
  </body>
</html></richcontent>
<arrowlink DESTINATION="ID_428365633" ENDARROW="Default" ENDINCLINATION="176;0;" ID="Arrow_ID_1501436647" STARTARROW="None" STARTINCLINATION="176;0;"/>
</node>
<node CREATED="1472498654730" ID="ID_1869339299" MODIFIED="1472498676884" TEXT="gel&#xf6;st durch static_assert">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1458178300984" FOLDED="true" ID="ID_1152672899" MODIFIED="1488423308083" TEXT="Binding-Layer">
<node CREATED="1458850722203" FOLDED="true" ID="ID_1565475995" MODIFIED="1488423308060" TEXT="Fehler bei fehlender Funktionalit&#xe4;t">
<node CREATED="1458850733769" ID="ID_764229197" MODIFIED="1458850744620" TEXT="kein Setter"/>
<node CREATED="1458850746184" ID="ID_105803291" MODIFIED="1458850752370" TEXT="kein Mutator"/>
</node>
<node CREATED="1458850782747" ID="ID_1019140953" MODIFIED="1458850793430" TEXT="Umgang mit fehlendem Selector"/>
<node CREATED="1458934854051" FOLDED="true" ID="ID_898160956" MODIFIED="1488423308060" TEXT="Binding st&#xe4;rker abschlie&#xdf;en">
<node CREATED="1458934863225" ID="ID_843139682" MODIFIED="1458934897237" TEXT="contentBuffer k&#xf6;nnte privat sein">
<icon BUILTIN="help"/>
</node>
<node CREATED="1458934878911" ID="ID_986928457" MODIFIED="1458934893132" TEXT="welche Iteratoren m&#xfc;ssen exponiert werden">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1460762013033" FOLDED="true" ID="ID_758721262" MODIFIED="1488423308060" TEXT="Mutation">
<node CREATED="1460762015777" FOLDED="true" ID="ID_59179895" MODIFIED="1488423307069">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erfordert <i>wirklich</i>&#160;Kooperation
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1460762025520" ID="ID_955853878" MODIFIED="1460762032675" TEXT="zwischen dem gesendeten Diff"/>
<node CREATED="1460762033119" ID="ID_863964003" MODIFIED="1460762045704" TEXT="und der empfangenden Implementierungs-Datenstruktur"/>
</node>
<node CREATED="1460762065371" FOLDED="true" ID="ID_698989532" MODIFIED="1488423307069" TEXT="brauche wohl &quot;Introspection light&quot;">
<icon BUILTIN="smily_bad"/>
<node CREATED="1460762101046" ID="ID_1593562435" MODIFIED="1460762111448" TEXT="damit ich einen Sub-Scope vorbereiten kann"/>
<node CREATED="1460762111924" ID="ID_125430023" MODIFIED="1460762120759" TEXT="in separater privater Datenstrukcur"/>
<node CREATED="1460762121259" ID="ID_1558869553" MODIFIED="1460762308964" TEXT="ist aber nur rein hier im Test ein Problem">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn wir verwenden hier als &quot;private&quot; Datenstruktur
    </p>
    <p>
      eine etwas komische Collection von Strings,
    </p>
    <p>
      in die wir die String-Repr&#228;sentation der Spec-Payload schreiben.
    </p>
    <p>
      
    </p>
    <p>
      In der Praxis dagegen w&#252;rde man wirklich einen privaten Datentyp verwenden,
    </p>
    <p>
      und dann auch voraussetzen, da&#223; man <i>nur</i>&#160;Kinder dieses Typs (oder zuweisungskompatibel) bekommt.
    </p>
    <p>
      
    </p>
    <p>
      Mein Poblem hier ist, da&#223; ich in dieser Demonstrations-Datenstruktur keine nested scopes repr&#228;sentieren kann.
    </p>
    <p>
      Aber hey!, es ist meine private Datenstruktur -- also kann ich einfach eine Map von nested scopes
    </p>
    <p>
      daneben auf die gr&#252;ne Wiese stellen. Ist ja nur ein Test :-D
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1460762330487" ID="ID_914518092" MODIFIED="1460762395843" TEXT="Nein! Schwein gehabt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...dankenswerterweise hat der subscript-Operator von std::Map
    </p>
    <p>
      die nette Eigenschaft, beim ersten Zugriff auf einen neuen Key
    </p>
    <p>
      dessen payload per default-konstruktor zu erzeugen.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
</node>
<node CREATED="1458868836883" FOLDED="true" ID="ID_717368167" MODIFIED="1488423308083" TEXT="Beobachtungen">
<node CREATED="1458868852849" ID="ID_1641586585" MODIFIED="1458869076689" TEXT="Sonderbarer &quot;this&quot;-Typ">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      der Builder in der nested DSL generiert einen sonderbar falschen &quot;this&quot;-Typ,
    </p>
    <p>
      genauer gesagt, eine TYPID die falsch ist.
    </p>
    <p>
      Und zwar kommt es da zum &quot;&#220;bersprechen&quot; von einem Typ-Parameter in den anderen.
    </p>
    <p>
      Im Besonderen hab ich beobachtet, da&#223;, wenn man auf den 3.Typparameter ein Lambda gibt,
    </p>
    <p>
      dann auf dem 4. oder 5. Typparameter der bisherige /alte Typ des 3.Typparameters auftaucht,
    </p>
    <p>
      u.U auch eingeschachtelt als ein Argument.
    </p>
    <p>
      
    </p>
    <p>
      Habe mich aber davon &#252;berzeugt, da&#223; die eigentlichen Typ-Parameter in Ordnung sind.
    </p>
    <p>
      Und zwar habe ich das verifiziert
    </p>
    <ul>
      <li>
        durch Ausgeben der Typen im Konstruktor (mithilfe meiner typeStr&lt;TY&gt;()
      </li>
      <li>
        durch Einbauen einer Static-Assertion mit Signatur-Match
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1458850516270" FOLDED="true" ID="ID_263851499" MODIFIED="1488423308083" TEXT="Test">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1458850521270" FOLDED="true" ID="ID_534213210" MODIFIED="1488423308060" TEXT="F&#xe4;lle">
<arrowlink COLOR="#639ad5" DESTINATION="ID_1333254858" ENDARROW="Default" ENDINCLINATION="1091;0;" ID="Arrow_ID_709688381" STARTARROW="None" STARTINCLINATION="1150;185;"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1458850526133" ID="ID_55717538" MODIFIED="1458850550109" TEXT="Binden an Sammlung aus Primitiven"/>
<node CREATED="1458850550705" ID="ID_1923906009" MODIFIED="1458850572315" TEXT="Binden an Sammlung aus Spezialtypen"/>
</node>
<node CREATED="1458850656668" FOLDED="true" ID="ID_307163685" MODIFIED="1488423308061" TEXT="Defaults">
<node CREATED="1458850660523" ID="ID_326530451" MODIFIED="1460753283653" TEXT="GenNode-vertr&#xe4;glicher Payload-Typ">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint ist:
    </p>
    <p>
      die native Datenstruktur ist eine Collection von Elementen,
    </p>
    <p>
      welche ohne Weiteres direkt in eine GenNode gepackt werden k&#246;nnten. Denn dann l&#228;&#223;t
    </p>
    <p>
      sich eine einfache Default-Implementierung des Matchers angeben
    </p>
    <p>
      
    </p>
    <p>
      Typisches Beispiel: eine STL-Collection von Strings.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458850672705" ID="ID_1240263288" MODIFIED="1458850675197" TEXT="kein Setter"/>
<node CREATED="1458850675641" ID="ID_736176562" MODIFIED="1458850678564" TEXT="kein Mutator"/>
</node>
</node>
</node>
</node>
<node CREATED="1457232752458" FOLDED="true" ID="ID_1490646673" MODIFIED="1488423308114" TEXT="Attribute">
<icon BUILTIN="full-3"/>
<node CREATED="1458175823858" FOLDED="true" ID="ID_1785794072" MODIFIED="1488423308099" TEXT="Design-Grundlagen">
<node CREATED="1461882686190" ID="ID_233456976" MODIFIED="1472219338718" TEXT="von praktischer Notwendigkeit getrieben">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wir integrieren Attribute nicht, weil es so sch&#246;n symmetrisch ist,
    </p>
    <p>
      sondern weil sie essentiell zum Wesen von Objekten geh&#246;ren.
    </p>
    <p>
      Wenn wir &#196;nderungen an Objekt-Strukturen als Diff erfassen wollen,
    </p>
    <p>
      dann <i>m&#252;ssen</i>&#160;Attribute irgendwie sinnvoll integriert sein
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1457741673001" FOLDED="true" ID="ID_1826667029" MODIFIED="1488423308083" TEXT="konkrete Beispiele">
<icon BUILTIN="info"/>
<node CREATED="1457741683687" ID="ID_1529941512" MODIFIED="1461888218005" TEXT="Position eines Clips"/>
<node CREATED="1461888218521" ID="ID_1211555549" MODIFIED="1461888222508" TEXT="L&#xe4;nge eines Clips"/>
<node CREATED="1461888223024" ID="ID_854712901" MODIFIED="1461888302874" TEXT="Typ eines Markers"/>
<node CREATED="1461888304126" ID="ID_854691640" MODIFIED="1461888305362" TEXT="..."/>
</node>
<node CREATED="1457742434714" ID="ID_1230374921" MODIFIED="1461888342584">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &#10233; <i>immer</i>&#160;in der Klasse verankert
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1457742434714" ID="ID_261686106" MODIFIED="1461888407600">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &#10233; es geht eigentlich <i>nur</i>&#160;um den Wert des Attributes
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1458175861176" FOLDED="true" HGAP="39" ID="ID_887227300" MODIFIED="1488423308099" TEXT="Design-Anforderungen" VSHIFT="7">
<node CREATED="1458175880034" FOLDED="true" ID="ID_163174135" MODIFIED="1488423308083" TEXT="...ein Objekt hat schon ein bestimmtes Feld">
<node CREATED="1458175892400" ID="ID_640596624" MODIFIED="1461888660658" TEXT="Typ"/>
<node CREATED="1461888661462" ID="ID_1138175982" MODIFIED="1461888668033" TEXT="default-Wert"/>
<node CREATED="1458175905766" ID="ID_1595003935" MODIFIED="1461888693671">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      manche Felder sind <i>optional</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1461888779999" FOLDED="true" ID="ID_1274350149" MODIFIED="1488423308084" TEXT="innerer Widerspruch">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1461888830680" ID="ID_812148839" MODIFIED="1461888837867" TEXT="Feld vs Attribut"/>
<node CREATED="1461888838455" ID="ID_1919503899" MODIFIED="1461888848953" TEXT="das Diff dr&#xfc;ckt sich in Attributen aus"/>
<node CREATED="1461888982036" FOLDED="true" ID="ID_864479720" MODIFIED="1488423308061" TEXT="Umgang mit den Diskrepanzen">
<icon BUILTIN="help"/>
<node CREATED="1461889012008" FOLDED="true" ID="ID_1889833023" MODIFIED="1488423307110" TEXT="Einf&#xfc;gen eines Attributes">
<node CREATED="1461889021646" ID="ID_1936260059" MODIFIED="1461889026273" TEXT="wo das Feld schon da ist"/>
<node CREATED="1461889026813" ID="ID_1307987216" MODIFIED="1461889040368" TEXT="wo es kein korrespondierendes Feld gibt"/>
</node>
<node CREATED="1461889146846" ID="ID_354158582" MODIFIED="1461889152792" TEXT="L&#xf6;schen eines Attributes"/>
<node CREATED="1461889107939" ID="ID_1744229690" MODIFIED="1461889133891" TEXT="Umordnen von Attributen"/>
</node>
<node CREATED="1461889187336" FOLDED="true" ID="ID_829644660" MODIFIED="1488423308061" TEXT="Leitgedanke">
<font NAME="SansSerif" SIZE="15"/>
<node CREATED="1461889203662" ID="ID_1763333007" MODIFIED="1461889243250" TEXT="&#xbb;auf GenNode und Objekt gleicherma&#xdf;en anwendbar&#xab;">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1461889253087" FOLDED="true" ID="ID_1128320234" MODIFIED="1488423307110" TEXT="das hei&#xdf;t...">
<font NAME="SansSerif" SIZE="11"/>
<node CREATED="1461889261806" ID="ID_1855892827" MODIFIED="1461889621538" TEXT="wende Diff auf GenNode an">
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1461889282580" ID="ID_1159487501" MODIFIED="1461889621538" TEXT="wende gleichen Diff auf gebundenes Objekt an">
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1461889296634" ID="ID_1774882278" MODIFIED="1461889621537" TEXT="Ergebnis mu&#xdf; semantisch &#xe4;quivalent sein">
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1461889399932" ID="ID_1052073468" MODIFIED="1461889621534">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      unter der Ma&#223;gabe,
    </p>
    <p>
      wie ETD ein Objekt rep&#228;sentiert
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1461889470866" FOLDED="true" ID="ID_404525574" MODIFIED="1488423307110" TEXT="operational">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1461889474402" ID="ID_1211544465" MODIFIED="1463672686487" TEXT="alle Informationen, die die Anwendung nicht nutzt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &quot;Anwendung&quot; : meint das Anwenden eines Diffs auf ein Ziel-Objekt
    </p>
    <p>
      &quot;nicht nutzen&quot; : meint ignorieren und verwerfen der Information
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461889550072" ID="ID_48030760" MODIFIED="1461889558442" TEXT="d&#xfc;rfen nur Zustands-&#xc4;nderungen bewirken"/>
<node CREATED="1461889558983" ID="ID_1489246908" MODIFIED="1461889796278" TEXT="welche keinen Einflu&#xdf; auf nachfolgende Diffs haben">
<arrowlink COLOR="#be5b66" DESTINATION="ID_781150142" ENDARROW="Default" ENDINCLINATION="28;-28;" ID="Arrow_ID_30022331" STARTARROW="None" STARTINCLINATION="13;20;"/>
</node>
</node>
</node>
<node CREATED="1461889656194" FOLDED="true" HGAP="73" ID="ID_781150142" MODIFIED="1488423307111" TEXT="ist das &#xfc;berhaupt m&#xf6;glich" VSHIFT="23">
<linktarget COLOR="#be5b66" DESTINATION="ID_781150142" ENDARROW="Default" ENDINCLINATION="28;-28;" ID="Arrow_ID_30022331" SOURCE="ID_1489246908" STARTARROW="None" STARTINCLINATION="13;20;"/>
<icon BUILTIN="help"/>
<node CREATED="1461889858863" ID="ID_1367853310" MODIFIED="1461889937247" TEXT="sicher nicht bidirektional">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      meint: ETD -&gt; Objekt und dann sp&#228;ter Objekt -&gt; ETD
    </p>
    <p>
      
    </p>
    <p>
      warum?
    </p>
    <p>
      Weil sich in der ETD die Reihenfolge &#228;ndern kann,
    </p>
    <p>
      und aber das Aufspielen eines Diffs auf beiden Seiten
    </p>
    <p>
      zwingend die gleiche Reihenfolge erfordert!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461890009355" ID="ID_1808149320" MODIFIED="1475506883423" TEXT="aber ETD als Zwischenstufe ist m&#xf6;glich">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Objekt -&gt; ETD -&gt; Objekt
    </p>
    <p>
      
    </p>
    <p>
      warum?
    </p>
    <p>
      weil das Quellobjekt keinen Diff erzeugen wird,
    </p>
    <p>
      der sich letztlich nicht auf das Zielobjekt aufspielen l&#228;&#223;t
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#807bce" DESTINATION="ID_1808149320" ENDARROW="Default" ENDINCLINATION="671;0;" ID="Arrow_ID_1677374853" SOURCE="ID_1767038604" STARTARROW="Default" STARTINCLINATION="994;0;"/>
</node>
<node CREATED="1461890240148" FOLDED="true" ID="ID_1409253452" MODIFIED="1488423307110" TEXT="Schutzmechanismen">
<node CREATED="1461890266377" ID="ID_513997188" MODIFIED="1461890283701">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      abweisen, was das Kriterium <i>sicher verletzt</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461890289662" ID="ID_1663916739" MODIFIED="1461890307247" TEXT="Einf&#xfc;gen eines Feldes, das die Klasse nicht kennt"/>
<node CREATED="1461891798533" ID="ID_748156171" MODIFIED="1461891807616" TEXT="explizites Umordnen (per seek)"/>
</node>
</node>
</node>
</node>
<node CREATED="1461892011913" FOLDED="true" HGAP="227" ID="ID_1407170666" MODIFIED="1488423308084" TEXT="Verhaltensmuster" VSHIFT="11">
<node CREATED="1461892073089" FOLDED="true" ID="ID_883084327" MODIFIED="1488423308061" TEXT="default vs mandatory">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>mandatory</b>&#160;: Wert mu&#223; per Konstruktor gegeben sein
    </p>
    <p>
      <b>default</b>&#160;: es gibt einen ausgezeichneten Standardwert
    </p>
  </body>
</html></richcontent>
<node CREATED="1461892285965" FOLDED="true" ID="ID_440312526" MODIFIED="1488423307111" TEXT="mandatory ">
<node CREATED="1461892303666" ID="ID_1486672593" MODIFIED="1461892312404" TEXT="Problem der Konstruktor-Repr&#xe4;sentation"/>
<node CREATED="1461892313049" ID="ID_683528850" MODIFIED="1461892394247" TEXT="einziger Ausweg: komplette ETD-Repr&#xe4;sentation als Wert senden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, in dem ins-Verb ist dann ein komplettes Objekt enthalten,
    </p>
    <p>
      nicht nur eine leere Record-H&#252;lle, die nachfolgend populiert werden kann (aber nicht mu&#223;)
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461892430466" ID="ID_1727811732" MODIFIED="1461892440092" TEXT="ein mandatory-Feld darf nicht gel&#xf6;scht werden"/>
</node>
<node CREATED="1461892444727" FOLDED="true" ID="ID_1828810203" MODIFIED="1488423307111" TEXT="default">
<node CREATED="1461892490881" FOLDED="true" ID="ID_577669206" MODIFIED="1488423307111" TEXT="es gibt zwei Grade">
<node CREATED="1461892496377" ID="ID_1216553390" MODIFIED="1461892579692" TEXT="minder">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Konstruktor bef&#252;llt das Feld halt irgendwie.
    </p>
    <p>
      Ab dem Punkt verh&#228;lt es sich aber wie ein normales (mandatory) Feld
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461892554377" ID="ID_1564116180" MODIFIED="1461892615511" TEXT="vollwertig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das Objekt selber kann erkennen, ob das Feld sich im &quot;default-Zustand&quot; befindet
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1461892641949" ID="ID_600125030" MODIFIED="1461892653415" TEXT="Einf&#xfc;gen == vom Default-Wert abweichen"/>
<node CREATED="1461892654148" ID="ID_1086408965" MODIFIED="1461892662590" TEXT="L&#xf6;schen == zum Default-Wert zur&#xfc;ckkehren"/>
</node>
</node>
<node CREATED="1461966988471" FOLDED="true" ID="ID_751317533" MODIFIED="1488423308061" TEXT="pr&#xfc;fen vs ignorieren">
<node CREATED="1461967012164" FOLDED="true" ID="ID_1380142317" MODIFIED="1488423307111" TEXT="pr&#xfc;fen">
<icon BUILTIN="forward"/>
<node CREATED="1461967015883" ID="ID_1980878581" MODIFIED="1461967028022" TEXT="ben&#xf6;tigt zus&#xe4;tzliche Mechanismen"/>
<node CREATED="1461967028489" ID="ID_1694830814" MODIFIED="1461967042164" TEXT="verifiziert Validit&#xe4;t der Diff-Nachricht"/>
</node>
<node CREATED="1461967044480" FOLDED="true" ID="ID_810828038" MODIFIED="1488423307111" TEXT="ignorieren">
<icon BUILTIN="button_cancel"/>
<node CREATED="1461967051095" ID="ID_328050724" MODIFIED="1461967056946" TEXT="wendet an, was anwendbar ist"/>
<node CREATED="1461967057462" ID="ID_437076290" MODIFIED="1461967068248" TEXT="alles andere &quot;durchwinken&quot;"/>
</node>
<node COLOR="#990000" CREATED="1461967003141" FOLDED="true" HGAP="29" ID="ID_1805886948" MODIFIED="1488423307112" TEXT="Design-Entscheidung" VSHIFT="19">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1461967143851" ID="ID_1596604972" MODIFIED="1461967245047">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ohne Pr&#252;fen ist <b>emptySrc</b>&#160;nicht implementierbar
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...weil es f&#252;r <b>emptySrc</b>&#160;keine neutrale Antwort gibt.
    </p>
    <p>
      Denn dieses Pr&#228;dikat wird von der typischen Implementierung des Diff-Applikators
    </p>
    <p>
      in beiden Richtungen verwendet, also sowohl Pr&#252;fung auf empty (&quot;expect no further elements&quot;),
    </p>
    <p>
      alsauch der Check, da&#223; &#252;berhaupt noch Quellelemente anstehen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461967247093" ID="ID_366726823" MODIFIED="1461967294701">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h., man kann nur <i>global auf Pr&#252;fung verzichten&#160;</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461967274577" ID="ID_1824726078" MODIFIED="1461967310565">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und da habe ich mich bereits <i><font color="#d3204b">dagegen entschieden</font></i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461967364397" ID="ID_381561226" MODIFIED="1461967379991" TEXT="Pr&#xfc;fung erfordert eine Flag f&#xfc;r jedes bekannte Feld"/>
<node CREATED="1461967384579" ID="ID_1805227176" MODIFIED="1461967401644" TEXT="diese Flag wird gesetzt, sobald der Diff das Feld &quot;ber&#xfc;hrt&quot;"/>
<node CREATED="1461967428061" FOLDED="true" ID="ID_766802419" MODIFIED="1488423307111" TEXT="au&#xdf;erdem gibt es noch zwei triviale Basis-Checks">
<node CREATED="1461967451610" ID="ID_798276463" MODIFIED="1461967462700" TEXT="ist bekannter Attributsname"/>
<node CREATED="1461967463264" ID="ID_1109004010" MODIFIED="1461967487243">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Feld unterst&#252;tzt <b>default</b>-Wert
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1475506680125" FOLDED="true" HGAP="58" ID="ID_1865970661" MODIFIED="1488423307112" TEXT="Variante" VSHIFT="13">
<node CREATED="1475506691595" ID="ID_69735021" MODIFIED="1475506704861" TEXT="emptySrc -&gt; hasSrc"/>
<node CREATED="1475506705641" ID="ID_409132685" MODIFIED="1475506735169" TEXT="und den nicht implementierbaren Fall tolerieren"/>
<node CREATED="1475506735949" ID="ID_1814707193" MODIFIED="1475506746633" TEXT="im &#xdc;brigen: stets pr&#xfc;fen und scheitern">
<icon BUILTIN="yes"/>
</node>
<node COLOR="#338800" CREATED="1475506753523" HGAP="34" ID="ID_845946083" MODIFIED="1475506765947" TEXT="so umgesetzt" VSHIFT="10">
<font NAME="SansSerif" SIZE="10"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node CREATED="1455927276314" FOLDED="true" HGAP="100" ID="ID_98863612" MODIFIED="1488423308084" TEXT="Spezifikation" VSHIFT="18">
<cloud COLOR="#fce9c0"/>
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="pencil"/>
<node CREATED="1461946125744" FOLDED="true" HGAP="55" ID="ID_1970709109" MODIFIED="1488423308061">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Auslegung der
    </p>
    <p>
      <font size="5">Primitiven</font>
    </p>
  </body>
</html></richcontent>
<node CREATED="1455927425726" FOLDED="true" ID="ID_1774195026" MODIFIED="1488423307112" TEXT="hasSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928216420" ID="ID_259184763" MODIFIED="1464116966043" TEXT="can not sensibly be implemented">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1464116995888" ID="ID_892348638" MODIFIED="1464117033135" TEXT="always returns true">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1464116967676" ID="ID_1396378266" MODIFIED="1464117025435">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      rationale: object fields are hard wired,
    </p>
    <p>
      thus always available
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1455927425726" FOLDED="true" ID="ID_950488526" MODIFIED="1488423307112" TEXT="skipSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="closed"/>
<node CREATED="1464212421708" ID="ID_260959353" MODIFIED="1464212429644" TEXT="prohibited, raise error"/>
<node CREATED="1455928216420" ID="ID_1837102790" MODIFIED="1464212390729" TEXT="reset to default">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1461964714068" ID="ID_1261372831" MODIFIED="1463690748989" TEXT="mandatory fields raise error">
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1455927396505" FOLDED="true" ID="ID_1033919055" MODIFIED="1488423307112" TEXT="matchSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455928268589" ID="ID_954747330" MODIFIED="1461964779839" TEXT="ID comparison to field known by name"/>
</node>
<node CREATED="1455982947867" FOLDED="true" ID="ID_348226285" MODIFIED="1488423307112" TEXT="injectNew">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1455982969073" ID="ID_1599013380" MODIFIED="1463675987940" TEXT="set known field&apos;s contents"/>
<node CREATED="1457047512175" ID="ID_45133594" MODIFIED="1464212464658" TEXT="mandatory fields raise error">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1461964818054" ID="ID_1444647202" MODIFIED="1461964825281" TEXT="unknown fields raise error"/>
</node>
<node CREATED="1455927413191" FOLDED="true" ID="ID_131020445" MODIFIED="1488423307112" TEXT="acceptSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1464212476461" ID="ID_503509206" MODIFIED="1464212481606" TEXT="NOP"/>
<node CREATED="1461964863217" ID="ID_274118585" MODIFIED="1464212474879" TEXT="mark known field as mentioned">
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node CREATED="1455928166683" FOLDED="true" ID="ID_1715452226" MODIFIED="1488423307113" TEXT="accept_until">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="stop-sign"/>
<node CREATED="1455928383765" ID="ID_1566496401" MODIFIED="1461965340133" TEXT="nicht implementierbar">
<font BOLD="true" NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1457231714541" ID="ID_1922580473" MODIFIED="1465428399592" TEXT="fraglich: mu&#xdf; vielleicht aus der Sprache entfernt werden">
<arrowlink COLOR="#592379" DESTINATION="ID_693317592" ENDARROW="Default" ENDINCLINATION="577;0;" ID="Arrow_ID_1115282938" STARTARROW="Default" STARTINCLINATION="1205;231;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1463675830732" FOLDED="true" ID="ID_1893171223" MODIFIED="1488423307112">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Einschr&#228;nkung:&#160;<b>accept_until END</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1463675866200" ID="ID_410463345" MODIFIED="1463675944546" TEXT="ist implementierbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...n&#228;mlich indem alle Attribute als &quot;ber&#252;hrt&quot; und akzeptiert markiert werden.
    </p>
    <p>
      Somit k&#246;nnten sofort Zuweisungen als N&#228;chstes passieren
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463675872239" ID="ID_467354174" MODIFIED="1463675882809" TEXT="ist sehr sinnvoll"/>
<node CREATED="1463675883853" ID="ID_178701147" MODIFIED="1464212598924" TEXT="k&#xf6;nnte sogar Standardfall sein">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1464212588886" ID="ID_1883145154" MODIFIED="1464212594754" TEXT="ist effektiv NOP">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1455927413191" FOLDED="true" ID="ID_216877268" MODIFIED="1488423307113" TEXT="findSrc">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="closed"/>
<node CREATED="1461946965569" ID="ID_229931358" MODIFIED="1461963646230" TEXT="Umordnen wird nicht unterst&#xfc;tzt"/>
<node CREATED="1461963648481" ID="ID_1893776707" MODIFIED="1461963654996" TEXT="l&#xf6;st einen Logik-Fehler aus"/>
</node>
<node CREATED="1455928184504" FOLDED="true" ID="ID_850800360" MODIFIED="1488423307113" TEXT="assignElm">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1461963697714" ID="ID_1619863394" MODIFIED="1461963707499" TEXT="Setter aufrufen"/>
<node CREATED="1461963686556" ID="ID_662132890" MODIFIED="1461963697214" TEXT="namentlich bekanntes Feld"/>
<node CREATED="1461963708449" ID="ID_1854747377" MODIFIED="1461963718804" TEXT="nicht bekannte Felder l&#xf6;sen Fehler aus"/>
</node>
<node CREATED="1455928189695" FOLDED="true" ID="ID_1575249106" MODIFIED="1488423307113" TEXT="mutateChild">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1461963754651" ID="ID_843075689" MODIFIED="1461963776115">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      analog wie&#160;<b>assignElm</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461963748604" ID="ID_465316362" MODIFIED="1461963753895" TEXT="namentlich bekanntes Feld"/>
<node CREATED="1461963777440" ID="ID_207528376" MODIFIED="1461963787443" TEXT="aber nicht Setter, sondern Mutator bauen"/>
</node>
<node CREATED="1464113688645" FOLDED="true" ID="ID_1227423402" MODIFIED="1488423307113" STYLE="fork" TEXT="completeScope">
<edge COLOR="#808080" STYLE="bezier" WIDTH="thin"/>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1464113727192" ID="ID_656884609" MODIFIED="1464113954773" TEXT="hier NOP implementiert"/>
<node CREATED="1464113740005" ID="ID_1570177974" MODIFIED="1464114004153">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, es findet <b>keine Verifikation</b>&#160;statt
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1455928193863" FOLDED="true" ID="ID_579601999" MODIFIED="1488423307113" TEXT="CTOR">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1461946885292" ID="ID_319452226" MODIFIED="1461965407132" TEXT="mark all known fields as not yet mentioned"/>
<node COLOR="#e5500c" CREATED="1461946693701" ID="ID_583693798" MODIFIED="1461965425322" STYLE="fork" TEXT="meta-Operation">
<edge COLOR="#808080" STYLE="bezier" WIDTH="thin"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
<node CREATED="1461946112906" FOLDED="true" HGAP="14" ID="ID_388598327" MODIFIED="1488423308062" VSHIFT="21">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      zu bindende
    </p>
    <p>
      <font size="4">Operationen</font>
    </p>
  </body>
</html></richcontent>
<node CREATED="1458175991795" FOLDED="true" ID="ID_1544297658" MODIFIED="1488423307113" TEXT="fest eingebaut...">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458175942866" FOLDED="true" ID="ID_321386709" MODIFIED="1488423307113" TEXT="match">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177113846" ID="ID_885225658" MODIFIED="1461966728693" TEXT="pr&#xfc;ft ID-Name der Spec gegen Feldname des Zieles"/>
</node>
<node CREATED="1458176002762" FOLDED="true" ID="ID_233019425" MODIFIED="1488423307113" TEXT="selector">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177605869" ID="ID_1275056489" MODIFIED="1461966119517" TEXT="ein default-Selector ist automatisch stets konfiguriert"/>
<node CREATED="1461966120082" ID="ID_92075200" MODIFIED="1461966665670" TEXT="default ist Pr&#xfc;fen, ob die Spec ein Attribut bezeichnet"/>
</node>
</node>
<node CREATED="1458175991795" FOLDED="true" ID="ID_998867093" MODIFIED="1488423307115" TEXT="optional...">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458176008025" FOLDED="true" ID="ID_384595680" MODIFIED="1488423307114" TEXT="setter">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177697657" ID="ID_1292886690" MODIFIED="1458177953095" TEXT="erlaubt Zuweisung"/>
<node CREATED="1458177712798" ID="ID_1904681311" MODIFIED="1458177953095" TEXT="setzt Auswahl per match voraus"/>
<node CREATED="1458177735635" FOLDED="true" ID="ID_88342940" MODIFIED="1488423307114" TEXT="Ersatz">
<icon BUILTIN="button_cancel"/>
<node CREATED="1458177738499" ID="ID_738570524" MODIFIED="1458177953095" TEXT="keiner"/>
<node CREATED="1458177741155" ID="ID_504602301" MODIFIED="1458177953095" TEXT="dann gibt&apos;s eben keine Zuweisung"/>
<node CREATED="1458177749233" ID="ID_1737530420" MODIFIED="1458177953095" TEXT="auch nicht n&#xf6;tig bei sub-Scopes"/>
<node CREATED="1458177803730" ID="ID_1570261235" MODIFIED="1458177953095" TEXT="auch nicht n&#xf6;tig bei immutable values"/>
</node>
</node>
<node CREATED="1458176018087" FOLDED="true" ID="ID_280816523" MODIFIED="1488423307114" TEXT="mutator">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1458177821712" ID="ID_850096330" MODIFIED="1458177953095" TEXT="baut einen sub-Mutator f&#xfc;r einen nested scope"/>
<node CREATED="1458177851988" ID="ID_835071729" MODIFIED="1458177953095" TEXT="setzt Auswahl per match voraus"/>
<node CREATED="1458177857939" FOLDED="true" ID="ID_1231660130" MODIFIED="1488423307114" TEXT="Ersatz">
<icon BUILTIN="button_cancel"/>
<node CREATED="1458177861162" ID="ID_1234972537" MODIFIED="1458177953095" TEXT="keiner"/>
<node CREATED="1458177863506" ID="ID_668640033" MODIFIED="1463690716478" TEXT="dann gibts eben keine Behandlung von nested scopes"/>
<node CREATED="1458177874760" ID="ID_98241568" MODIFIED="1458177953095" TEXT="auch nicht n&#xf6;tig bei primitive values"/>
</node>
</node>
<node CREATED="1463687547746" FOLDED="true" ID="ID_241762171" MODIFIED="1488423307114" TEXT="defaulter">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1463687553569" ID="ID_695577132" MODIFIED="1463687602782" TEXT="veranla&#xdf;t R&#xfc;ckkehr zum default-Zustand"/>
<node CREATED="1463687629319" ID="ID_613393149" MODIFIED="1463687644409" TEXT="erfordert explizite Unterst&#xfc;tzung durch das Zielobjekt"/>
<node CREATED="1463687644973" ID="ID_1677329552" MODIFIED="1463687741843" TEXT="fragw&#xfc;rdig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      sieht nach Ober-engineering aus,
    </p>
    <p>
      zumal das erhebliche Statefulness bewirkt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1463687649812" FOLDED="true" ID="ID_450096384" MODIFIED="1488423307114" TEXT="Ersatz">
<icon BUILTIN="button_cancel"/>
<node CREATED="1463687668218" ID="ID_205365114" MODIFIED="1463687670469" TEXT="keiner"/>
<node CREATED="1463687672649" ID="ID_244442064" MODIFIED="1463687689779" TEXT="dann kann eben nur gebunden werden"/>
<node CREATED="1463687865175" ID="ID_1290398590" MODIFIED="1463687869987" TEXT="normalerweise verzichtbar"/>
</node>
</node>
<node CREATED="1458175960615" FOLDED="true" ID="ID_806805692" MODIFIED="1488423307115" TEXT="ctor">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1461966391478" ID="ID_110542982" MODIFIED="1461966404134" TEXT="wird dem Setter vorgeschaltet"/>
<node CREATED="1458177315803" ID="ID_1572942300" MODIFIED="1458177953094" TEXT="konstruiert aus der Spec ein neues Element"/>
<node CREATED="1461966815950" ID="ID_156023396" MODIFIED="1461966831208" TEXT="notwendig bei Attribut-Werten, die tats&#xe4;chlich Objekte sind"/>
<node CREATED="1458177335632" ID="ID_1991494041" MODIFIED="1458177953094" TEXT="mu&#xdf; typischerweise die Interna des Zieles kennen"/>
<node CREATED="1458177347622" FOLDED="true" ID="ID_1000370922" MODIFIED="1488423307114" TEXT="Ersatz">
<icon BUILTIN="idea"/>
<node CREATED="1458177489444" ID="ID_1463500618" MODIFIED="1461966901007">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      unterstelle Ziel als <i>konstruierbar</i>&#160;aus Payload
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1463691200369" ID="ID_542600281" MODIFIED="1463691259602" TEXT="&#xfc;berfl&#xfc;ssiges Konzept">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da effektiv bereits der Setter diese Funktionalit&#228;t enthalten kann und mu&#223;,
    </p>
    <p>
      denn der Setter nimmt eine GenNode
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1458176048564" FOLDED="true" HGAP="-11" ID="ID_1811113811" MODIFIED="1488423308084" TEXT="Variationen" VSHIFT="4">
<node CREATED="1464117242535" FOLDED="true" ID="ID_1487426510" MODIFIED="1488423308062" TEXT="Binden auf eine klassische struct">
<node CREATED="1464117391859" ID="ID_1783492476" MODIFIED="1464117396430" TEXT="Setter weisen einfach zu"/>
<node CREATED="1464117397026" ID="ID_1635700062" MODIFIED="1464117404773" TEXT="bereits die Neuanlage mu&#xdf; alles populieren"/>
</node>
<node CREATED="1464117250262" FOLDED="true" ID="ID_1846973342" MODIFIED="1488423308062" TEXT="Binden auf ein Objekt mit Initialisierungs-Semantik">
<node CREATED="1464117412896" ID="ID_1405299167" MODIFIED="1464117450662" TEXT="Erzeugungs-Diff mu&#xdf; mit den Anforderungen des Konstruktors harmonieren"/>
<node CREATED="1464117487974" ID="ID_1923901911" MODIFIED="1464117499776" TEXT="R&#xfc;ckkehr zum Default wird nicht unterst&#xfc;tzt / abgebildet"/>
</node>
<node CREATED="1464117317517" FOLDED="true" ID="ID_1395959611" MODIFIED="1488423308062" TEXT="Binden auf &quot;konzeptionelle&quot; Properties">
<node CREATED="1464117513722" ID="ID_101207705" MODIFIED="1464117537611" TEXT="der &quot;Key&quot; ist nur eine ID, um speziellen Zugriffsmechanismus auszul&#xf6;sen"/>
<node CREATED="1464117553885" ID="ID_1617675711" MODIFIED="1464117567271" TEXT="u.U ist man &#xfc;berhaupt nur an sub-Mutation interessiert"/>
</node>
</node>
</node>
<node CREATED="1458178113697" FOLDED="true" HGAP="61" ID="ID_38103760" MODIFIED="1488423308100" TEXT="Design-Schlu&#xdf;folgerungen" VSHIFT="48">
<node CREATED="1463687362843" FOLDED="true" HGAP="116" ID="ID_775516687" MODIFIED="1488423308084" TEXT="Eingrenzung" VSHIFT="-28">
<node CREATED="1463687373889" FOLDED="true" ID="ID_155516876" MODIFIED="1488423308062" TEXT="Art von Attributen">
<node CREATED="1463687402462" FOLDED="true" ID="ID_1119981147" MODIFIED="1488423307126" TEXT="Objekt-Feld">
<icon BUILTIN="forward"/>
<node CREATED="1463689795357" ID="ID_1774256004" MODIFIED="1463689801592" TEXT="kann nur zugewiesen werden"/>
<node CREATED="1463689912853" FOLDED="true" ID="ID_592726515" MODIFIED="1488423307126">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>injectNew</b>&#160;tolerieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1463690238314" ID="ID_1366435072" MODIFIED="1463690270664" TEXT="eigentlich redundant">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....man k&#246;nnte genausogut auch beim ersten Mal zuweisen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463690487032" ID="ID_1618528789" MODIFIED="1463690595034" TEXT="Aber: isomorph zu GenNode">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denn die Diff-Anwendung auf GenNode unterst&#252;tzt Zuweisung
    </p>
    <p>
      ausschlie&#223;lich bei schon existierenden Elementen. Demnach mu&#223; dort auch jedes Attribut
    </p>
    <ul>
      <li>
        entweder schon mit dem Konstruktor mit gegeben worden sein
      </li>
      <li>
        oder vorher einmal explizit eingef&#252;gt
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1463687408597" FOLDED="true" ID="ID_783243290" MODIFIED="1488423307127" TEXT="optional Property">
<icon BUILTIN="stop-sign"/>
<node CREATED="1463688083594" ID="ID_1885891636" MODIFIED="1463688087910" TEXT="fragw&#xfc;rdiger Nutzen"/>
<node CREATED="1463688088457" ID="ID_1722180442" MODIFIED="1463688100116" TEXT="mutet &quot;theoretisch&quot; an"/>
<node CREATED="1463688147802" FOLDED="true" ID="ID_1259353240" MODIFIED="1488423307127" TEXT="Alternative">
<node CREATED="1463688157472" ID="ID_1130562584" MODIFIED="1463688173746" TEXT="auf &quot;mindere defaults&quot; setzen"/>
<node CREATED="1463688215752" ID="ID_1340272500" MODIFIED="1463688230896" TEXT="kann man einfach mitlaufen lassen"/>
<node CREATED="1463688184269" ID="ID_1652024004" MODIFIED="1463688193863" TEXT="immer m&#xf6;gliche Degradierung des Konzepts"/>
<node CREATED="1463688232550" FOLDED="true" ID="ID_1079102386" MODIFIED="1488423307126" TEXT="fehlende Funktionalit&#xe4;t ersetzbar">
<icon BUILTIN="idea"/>
<node CREATED="1463688247980" ID="ID_720310622" MODIFIED="1463688253639" TEXT="durch komplettes Neukonstruieren"/>
<node CREATED="1463688254251" ID="ID_1166606306" MODIFIED="1463688262902" TEXT="durch eine explizite Reset-Methode"/>
</node>
</node>
</node>
<node CREATED="1463687451791" FOLDED="true" ID="ID_682582613" MODIFIED="1488423307127" TEXT="Attribut-Map">
<icon BUILTIN="button_cancel"/>
<node CREATED="1463687461925" ID="ID_39763189" MODIFIED="1463687496231" TEXT="hier ausgeschlossen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1463687466573" ID="ID_1853473792" MODIFIED="1475507021278" TEXT="implementierbar auf Basis des Collection-Binding">
<linktarget COLOR="#9f96d9" DESTINATION="ID_1853473792" ENDARROW="Default" ENDINCLINATION="41;-121;" ID="Arrow_ID_503281678" SOURCE="ID_1112268061" STARTARROW="Default" STARTINCLINATION="1873;0;"/>
</node>
<node CREATED="1463687484338" ID="ID_1009085517" MODIFIED="1463687491613" TEXT="k&#xf6;nnte f&#xfc;r GenNode relevant sein"/>
</node>
</node>
<node CREATED="1463691267608" FOLDED="true" ID="ID_1554269824" MODIFIED="1488423308062" TEXT="Typisierung">
<node CREATED="1463691273431" ID="ID_1711015726" MODIFIED="1463691276947" TEXT="pro Attribut"/>
<node CREATED="1463691277726" FOLDED="true" ID="ID_716698874" MODIFIED="1488423307127" TEXT="implizit">
<node CREATED="1463691290845" ID="ID_1981578828" MODIFIED="1464226408020" TEXT="das Setter-lambda nimmt einen Wert"/>
<node CREATED="1464226412567" FOLDED="true" ID="ID_1195950141" MODIFIED="1488423307127" TEXT="der Wert-Typ mu&#xdf; einer der GenNode-Payload-Typen sein">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1464227158027" ID="ID_262712037" MODIFIED="1464227176058" TEXT="TODO: dies zur compile-Zeit pr&#xfc;fen">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1463691359988" ID="ID_139397516" MODIFIED="1464226506135" TEXT="der Setter mu&#xdf; jedwede weitere Konvertierung in andere Typen selber machen"/>
<node CREATED="1464226461041" ID="ID_401616087" MODIFIED="1464226487843" TEXT="die Implementierung greift auf den passenden GenNode-Payload zu"/>
<node CREATED="1463691372258" ID="ID_1648677750" MODIFIED="1463691550094" TEXT="alle Typkonvertierungs-Fragen auf client abgew&#xe4;lzt"/>
<node CREATED="1463691552514" ID="ID_1882798051" MODIFIED="1463691645252" TEXT="das ist gut so">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn wir vermeiden dadurch Komplexit&#228;t.
    </p>
    <p>
      Der gesendete Diff mu&#223; einfach passen!
    </p>
    <p>
      Genau deshalb haben wir auch in GenNode verschiedene Varianten des gleichen Grundtyps,
    </p>
    <p>
      damit wir nicht in die ganzen Ungewissheiten der widening conversions laufen!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
</node>
<node CREATED="1463691680849" FOLDED="true" ID="ID_909001007" MODIFIED="1488423308062" TEXT="Striktheit">
<node CREATED="1463691716821" ID="ID_414439233" MODIFIED="1463691724243" TEXT="soweit einfach zu gew&#xe4;hrleisten">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1463691725515" ID="ID_947499289" MODIFIED="1463691741484" TEXT="d.h. soweit nicht zus&#xe4;tzlicher State notwendig ist"/>
<node CREATED="1463691767421" ID="ID_993508973" MODIFIED="1463691780901" TEXT="Flexibilit&#xe4;t mu&#xdf; stets praktisch gerechtfertigt sein">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1463691793010" FOLDED="true" ID="ID_1079571503" MODIFIED="1488423308063" TEXT="F&#xe4;lle">
<node CREATED="1463691796825" ID="ID_1459507376" MODIFIED="1463691928935" TEXT="Setter">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. der Attributwert hat Wertsemantik und wird einfach zugewiesen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463691804696" ID="ID_1039735883" MODIFIED="1463692035179" TEXT="Mutator">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...d.h. der Attributwert ist ein Objekt und damit ein nested Scope
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463691807824" ID="ID_175488317" MODIFIED="1463691814860" TEXT="keine Mischformen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1463692054415" FOLDED="true" ID="ID_1755055624" MODIFIED="1464226580283">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem: <i>immutable values</i>
    </p>
  </body>
</html></richcontent>
<node CREATED="1463692505987" ID="ID_113919280" MODIFIED="1463692515040" TEXT="wirklich ein Problem?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1463692519993" ID="ID_1177616097" MODIFIED="1463692550512" TEXT="Diff handelt ohnehin nur von ver&#xe4;nderbaren Dingen"/>
<node CREATED="1463692663853" FOLDED="true" ID="ID_953408463" MODIFIED="1488423307127" TEXT="Aber: wie &#xe4;ndert man die L&#xe4;nge eins Clips im UI?">
<node CREATED="1463695438538" ID="ID_744412186" MODIFIED="1463695450620" TEXT="indem man sich auf ein Pr&#xe4;sentations-Grid bezieht"/>
<node CREATED="1463695454504" ID="ID_945765975" MODIFIED="1463695469602" TEXT="und folglich nur Grid-Punkt-Koordinaten &#xe4;ndert"/>
<node CREATED="1463695472134" ID="ID_852324151" MODIFIED="1463695494714" TEXT="die eigentlichen Time-Entit&#xe4;ten leben in der Session, nicht im UI">
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1463695573392" FOLDED="true" ID="ID_1066626880" MODIFIED="1488423307127" TEXT="im &#xdc;brigen: Problem des client-Codes">
<node CREATED="1463695588814" ID="ID_1661716485" MODIFIED="1463695605096" TEXT="man kann einen time::Mutator erzeugen"/>
<node CREATED="1463695605708" ID="ID_480704465" MODIFIED="1463695620401" TEXT="es gibt vorgefertigte EmbeddedMutators">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1463695633144" ID="ID_1677066412" MODIFIED="1463695657196">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das alles passiert dann <i>im</i>&#160;Lambda
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463695640231" ID="ID_176647136" MODIFIED="1463695645090" TEXT="und ist somit nicht unser Problem"/>
</node>
</node>
<node CREATED="1463849505993" FOLDED="true" HGAP="42" ID="ID_493843802" MODIFIED="1464226577515" VSHIFT="25">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Dilemma:&#160;<i>defaultable fields</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1463849536789" FOLDED="true" ID="ID_864537854" MODIFIED="1488423307127" TEXT="ich m&#xf6;chte sie unterst&#xfc;tzen">
<node CREATED="1463849545196" ID="ID_1187170275" MODIFIED="1463849547316" TEXT="warum">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1463849556474" ID="ID_73579891" MODIFIED="1463849682530" TEXT="Unterst&#xfc;tzung mu&#xdf; konsistent sein">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....mit der ETD,
    </p>
    <p>
      bzw der Anwendung des selben Diffs auf eine GenNode-Struktur.
    </p>
    <p>
      
    </p>
    <p>
      Konsequenz: wenn ein feld <i>defaulted</i>&#160;war, und nun explizit gesetzt wird,
    </p>
    <p>
      mu&#223; dies als INS geschehen, denn eine Zuweisung an nicht aufgef&#252;hrtes Element ist verboten
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463849716869" ID="ID_712482848" MODIFIED="1463849742347" TEXT="in der Bindung auf ein Objekt habe ich aber keine Attribut-Menge">
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1463849761127" ID="ID_865585394" MODIFIED="1463849778956">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      folglich ein Problem,
    </p>
    <p>
      zu erkennen, wenn wir fertig sind
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1463849986345" FOLDED="true" ID="ID_1305704801" MODIFIED="1488423307128" TEXT="genauer...">
<node CREATED="1463850017285" ID="ID_656883117" MODIFIED="1463850033879" TEXT="wenn ein feld zwar gebunden ist"/>
<node CREATED="1463850034699" ID="ID_1531134347" MODIFIED="1463850053756" TEXT="aber bisher keinen Wert bekommen hat"/>
<node CREATED="1463850060359" ID="ID_1083372309" MODIFIED="1463850081800" TEXT="dann darf der Diff es auch nicht per pick auff&#xfc;hren"/>
<node CREATED="1463850086044" ID="ID_1096436471" MODIFIED="1463850099574" TEXT="wenn dann alle mandatory fields behandelt sind"/>
<node CREATED="1463850100634" ID="ID_1329120598" MODIFIED="1463850243005" TEXT="glauben wir, wir sind noch nicht fertig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....weil das defaultable field noch nicht vom Diff ber&#252;hrt wurde.
    </p>
    <p>
      Aber es ist kein optional field, d.h. wir haben keine Flag, die es als &quot;defaulted&quot; kennzeichnet
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1463850138853" FOLDED="true" ID="ID_1262990387" MODIFIED="1488423307128" TEXT="also ist das Grundproblem">
<icon BUILTIN="idea"/>
<node CREATED="1463850144180" ID="ID_103581818" MODIFIED="1463850157454" TEXT="da&#xdf; der TreeMutator global stateless ist"/>
<node CREATED="1463850163257" ID="ID_1219321798" MODIFIED="1463850171716" TEXT="denn er wird immer wieder neu konstruiert"/>
<node CREATED="1463850280082" ID="ID_1381914386" MODIFIED="1463850294324">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      L&#246;sung: <b>alles immer explizit</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1463934907474" FOLDED="true" ID="ID_761652333" MODIFIED="1488423307128" TEXT="Auswege">
<node CREATED="1463934969522" FOLDED="true" ID="ID_960243774" MODIFIED="1488423307128" TEXT="keine optional fields">
<node COLOR="#a0345c" CREATED="1463937048979" ID="ID_861548194" MODIFIED="1463937120794" TEXT="widerspricht den Anforderungen">
<icon BUILTIN="stop-sign"/>
</node>
</node>
<node CREATED="1463935105351" FOLDED="true" ID="ID_1357485921" MODIFIED="1488423307128" TEXT="Protokoll-Auflagen">
<node CREATED="1463935116670" ID="ID_1375491306" MODIFIED="1463935125304" TEXT="nach dem ersten Durchgang mu&#xdf; alles gebunden sein"/>
<node CREATED="1463935126461" ID="ID_774412987" MODIFIED="1463935247552" TEXT="wenn ungebundene optionals, stets Abschlie&#xdf;en mit &quot;after END&quot;"/>
<node COLOR="#a0345c" CREATED="1463937146214" ID="ID_1154054760" MODIFIED="1463937452255" TEXT="attraktiv, aber unehrlich">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      diese L&#246;sung war zun&#228;chst mein Favorit.
    </p>
    <p>
      Sie erscheint sehr elegant, weil man im TreeMutator <i>&#252;berhaupt nichts</i>&#160;daf&#252;r tun mu&#223;.
    </p>
    <p>
      Und die Zusatz-Forderung, da&#223; dann eben das Diff richtig gesendet werden mu&#223;,
    </p>
    <p>
      erscheint &quot;geschenkt&quot;, da wir ohnehin zun&#228;chst einmal die Diffs explizit im Code erzeugen.
    </p>
    <p>
      
    </p>
    <p>
      Aber, nach l&#228;ngerer &#220;berlegung wurde mir der Ansatz mehr und mehr zweifelhaft.
    </p>
    <p>
      Das ist die Art von Verkoppelungen, hier die implizite Annahme einer bestimmten Implementierung,
    </p>
    <p>
      die ein System unerkl&#228;rbar und schwer wartbar machen. Das ist die Art von &quot;Features&quot;,
    </p>
    <p>
      f&#252;r die man sich nach einiger Zeit entschuldigen mu&#223;.
    </p>
    <p>
      
    </p>
    <p>
      Und noch schlimmer: eigentlich l&#228;uft dieser Ansatz darauf hinaus, die Konsistenzpr&#252;fung
    </p>
    <p>
      am Ende zu deaktivieren. Nur wir machen das nicht explizit, sondern durch die Hintert&#252;r.
    </p>
    <p>
      Also dann besser klar und ehrlich!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="stop-sign"/>
</node>
</node>
<node CREATED="1463934995110" FOLDED="true" ID="ID_1084188576" MODIFIED="1488423307128" TEXT="dem Mutator sagen, was optional ist">
<node CREATED="1463935368308" ID="ID_1008223435" MODIFIED="1463935375919" TEXT="was grunds&#xe4;tzlich optional sein kann"/>
<node CREATED="1463935377123" ID="ID_970486772" MODIFIED="1463935387005" TEXT="was aktuell nicht gebunden ist"/>
<node COLOR="#a0345c" CREATED="1463937178689" ID="ID_1161021042" MODIFIED="1463937550363" TEXT="Over-Engineering / L&#xf6;sung eines nicht vorhandenen Problems">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn unter dem Strich w&#252;rden wir hiermit volle Unterst&#252;tzung f&#252;r opitonale Attribute einf&#252;hren,
    </p>
    <p>
      also eine Attribut-Semantik auf eine Feld-Semantik draufpflanzen.
    </p>
    <p>
      Aber in der vorausgegangenen Analyse habe ich mich schon davon &#252;berzeugt,
    </p>
    <p>
      da&#223; wir keine Attribut-Semantik brauchen. Und wenn doch, dann bietet das Diff-System
    </p>
    <p>
      immer noch die M&#246;glichkeit, die Attribute explizit als Sammlung darzustellen.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="stop-sign"/>
</node>
</node>
<node CREATED="1463935030953" ID="ID_540672245" MODIFIED="1463938073583">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      auf die <i>empty</i>-Pr&#252;fung am Ende verzichten
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#f36478" DESTINATION="ID_1554206938" ENDARROW="Default" ENDINCLINATION="181;0;" ID="Arrow_ID_389060908" STARTARROW="None" STARTINCLINATION="181;0;"/>
<icon BUILTIN="forward"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1463937883299" FOLDED="true" HGAP="85" ID="ID_271104970" MODIFIED="1488423307129" TEXT="Konsequenzen" VSHIFT="26">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1463937922318" FOLDED="true" ID="ID_74130402" MODIFIED="1488423307128" TEXT="isEmpty">
<node CREATED="1463937972255" ID="ID_1554206938" MODIFIED="1463938146376" TEXT="mu&#xdf; umdefiniert werden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denn in den meisten, wichtigsten F&#228;llen get es um einen non-empty-check,
    </p>
    <p>
      bevor ein anderes Verifikations-Pr&#228;dikat angewendet wird.
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#f36478" DESTINATION="ID_1554206938" ENDARROW="Default" ENDINCLINATION="181;0;" ID="Arrow_ID_389060908" SOURCE="ID_540672245" STARTARROW="None" STARTINCLINATION="181;0;"/>
</node>
<node CREATED="1463937980222" ID="ID_1669470430" MODIFIED="1463937986216" TEXT="kann nicht mehr negativ gepr&#xfc;ft werden"/>
<node CREATED="1463937988180" ID="ID_1353983852" MODIFIED="1463938000454" TEXT="das gilt leider f&#xfc;r alle Bindings"/>
<node CREATED="1464212233893" ID="ID_1520677416" MODIFIED="1464212254420" TEXT="umdrehen in: hasSrc">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1463938033527" FOLDED="true" ID="ID_165279544" MODIFIED="1488423307128" TEXT="neue Primitive: completeScope">
<icon BUILTIN="button_ok"/>
<node CREATED="1464113590258" ID="ID_1613216716" MODIFIED="1464113600260" TEXT="auch ein hook f&#xfc;r Aufr&#xe4;um-Arbeiten"/>
<node CREATED="1464113600897" ID="ID_1783943539" MODIFIED="1464113642335" TEXT="return true -- wenn alles sauber ist"/>
<node CREATED="1464113642899" ID="ID_468050192" MODIFIED="1464113648966" TEXT="kann NOP implementiert werden"/>
</node>
</node>
<node CREATED="1464114047781" FOLDED="true" ID="ID_669832549" MODIFIED="1488423307128" TEXT="reduzierte Verifikation">
<icon BUILTIN="yes"/>
<node CREATED="1464114078672" ID="ID_72653804" MODIFIED="1464114085171" TEXT="nicht bekannte Felder werden erkannt"/>
<node CREATED="1464114086111" ID="ID_518116356" MODIFIED="1464114139188" TEXT="der Versuch von Umordnungen wird erkannt"/>
<node COLOR="#990000" CREATED="1464114156574" ID="ID_408810628" MODIFIED="1464114190655" TEXT="fehlende Angaben werden nicht bemerkt"/>
</node>
</node>
</node>
</node>
<node CREATED="1461897111747" FOLDED="true" HGAP="79" ID="ID_1690728106" MODIFIED="1488423308084" TEXT="Ein Attribut oder viele Attribute" VSHIFT="17">
<icon BUILTIN="help"/>
<node CREATED="1463696053440" ID="ID_564525411" MODIFIED="1464226693401" TEXT="Standardfall: eine kleine Zahl von Attributen ">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1463696031715" ID="ID_1300041378" MODIFIED="1464226781810" TEXT="&quot;ein Attribut&quot; ist aber der nat&#xfc;rliche Basisfall f&#xfc;r die Implementierung"/>
<node CREATED="1463695809296" ID="ID_780490723" MODIFIED="1464226736308" TEXT="problematisch sind Selektor und (ignorierte) Reihenfolge">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1464226806914" FOLDED="true" HGAP="25" ID="ID_1103913128" MODIFIED="1488423308084" TEXT="Selector" VSHIFT="5">
<icon BUILTIN="button_cancel"/>
<node CREATED="1464226813953" ID="ID_29622528" MODIFIED="1464226834307" TEXT="fest verdrahtet: istAttribut"/>
<node CREATED="1464226834831" ID="ID_585845912" MODIFIED="1464226848617" TEXT="Verzicht auf einen konfigurierbaren Selector"/>
<node CREATED="1464226849277" ID="ID_449408444" MODIFIED="1464226870862" TEXT="im Zweifelsfall: anderen &#xbb;onion-Layer&#xab; verwenden"/>
<node CREATED="1464226871930" ID="ID_569490118" MODIFIED="1464226897116" TEXT="gut zur Reduktion der Komplexit&#xe4;t">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1461894614295" FOLDED="true" ID="ID_1919790561" MODIFIED="1488423308084" TEXT="Attribut-Glieder">
<node CREATED="1461894619054" FOLDED="true" ID="ID_1860006379" MODIFIED="1488423308063" TEXT="implementieren die API-Primitive">
<node CREATED="1461894676063" ID="ID_1773874682" MODIFIED="1461894678714" TEXT="durch Aktion"/>
<node CREATED="1461894679190" ID="ID_1990110249" MODIFIED="1461894683857" TEXT="durch Ignorieren"/>
<node CREATED="1461894684326" ID="ID_744174163" MODIFIED="1461894692888" TEXT="als Fehler"/>
</node>
<node CREATED="1461894634029" ID="ID_1447610701" MODIFIED="1464226938524" TEXT="f&#xfc;gen sich nat&#xfc;rlich durch Layering zusammen"/>
<node CREATED="1461894666248" ID="ID_1972218753" MODIFIED="1464226987286" TEXT="Setter / Mutator durch Template-Spezialisierung"/>
<node CREATED="1464226992090" FOLDED="true" ID="ID_632987149" MODIFIED="1488423308063" TEXT="lineare Suche">
<icon BUILTIN="button_ok"/>
<node CREATED="1464227021205" ID="ID_1138709182" MODIFIED="1464227032960" TEXT="akzeptabel f&#xfc;r &lt; 30 Attribute"/>
<node CREATED="1464227042771" ID="ID_1594139733" MODIFIED="1464227114093" TEXT="vermeidet heap-basierte Collection">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      jedwede &quot;bessere&quot; Implementierung mu&#223; zwingend einen Container verwenden,
    </p>
    <p>
      der dann die Lambdas f&#252;r die einzelnen Setter auf den Heap legt.
    </p>
    <p>
      Das ist hier tats&#228;chlich viel schlechter, als das bischen lineare Suche
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1464227213180" FOLDED="true" HGAP="29" ID="ID_691018374" MODIFIED="1488423308100" TEXT="Implementierung" VSHIFT="66">
<icon BUILTIN="button_ok"/>
<node CREATED="1464227218795" FOLDED="true" ID="ID_1100206442" MODIFIED="1488423308084" TEXT="ist grunds&#xe4;tzlich &quot;schon da&quot;">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....durch meinen allerersten Draft,
    </p>
    <p>
      f&#252;r den ich damals gezwungen war, die GenNode zu erfinden :)
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node COLOR="#338800" CREATED="1464227273572" ID="ID_1669070695" MODIFIED="1465428151809" TEXT="TODO: alten Testfall anpassen">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1464227285562" ID="ID_908196685" MODIFIED="1465428159480" TEXT="TODO: Setter aus erstem Entwurf vom TreeMutator-API entfernen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1464227227610" ID="ID_392179794" MODIFIED="1464227235981" TEXT="aber: alle Primitive durchimplementieren"/>
<node COLOR="#338800" CREATED="1464386998240" FOLDED="true" ID="ID_1575821815" MODIFIED="1488423308085" TEXT="key: EntryID">
<icon BUILTIN="button_ok"/>
<node CREATED="1464396284551" FOLDED="true" ID="ID_608089097" MODIFIED="1488423308063" TEXT="Problem: sanitise">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1464396756424" ID="ID_1262469672" MODIFIED="1464397132659" TEXT="nur US-ASCII">
<icon BUILTIN="info"/>
</node>
<node CREATED="1464396761967" ID="ID_290057129" MODIFIED="1464397146966" TEXT="erweiterte Zeichen bedingen Locale">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1464396769974" FOLDED="true" ID="ID_1758045716" MODIFIED="1488423307136" TEXT="und welche dann??">
<node CREATED="1464396835581" ID="ID_1451158687" MODIFIED="1464396859658" TEXT="System-Locale: Recipe for Desaster"/>
<node CREATED="1464396793123" ID="ID_1031603106" MODIFIED="1464396815972" TEXT="en_US.UTF-8"/>
<node CREATED="1464396816696" ID="ID_75103205" MODIFIED="1464396866584" TEXT="Einschr&#xe4;nkung nicht sinnvoll zu begr&#xfc;nden">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1464396320922" FOLDED="true" ID="ID_1981813349" MODIFIED="1488423308063" TEXT="GenNode umgeht das (bewu&#xdf;t)">
<node CREATED="1464396885391" ID="ID_1510453315" MODIFIED="1464397164031" TEXT="Begr&#xfc;ndung: interne Schnittstelle">
<icon BUILTIN="info"/>
</node>
<node CREATED="1464396899252" ID="ID_1555498938" MODIFIED="1464396964843" TEXT="verwendet bereits valide Symbole">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1464396908371" ID="ID_487257918" MODIFIED="1464397170109" TEXT="vermeidet den Performance overhead">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1464397008078" ID="ID_1403561457" MODIFIED="1464397019505" TEXT="soll sich symmetrisch verhalten">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1464397020996" FOLDED="true" ID="ID_1333087135" MODIFIED="1488423308063" TEXT="L&#xf6;sung: Variante f&#xfc;r EntryID">
<icon BUILTIN="idea"/>
<node CREATED="1464397032747" ID="ID_1595458691" MODIFIED="1464397057946" TEXT="f&#xfc;r interne Schnittstellen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gleiches Argument...
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1464397039834" ID="ID_1349501863" MODIFIED="1464397115277" TEXT="verwendet Symbol-Datentyp">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...damit unterstellen wir, da&#223; sp&#228;ter eine Symbol-Tabelle aufgebaut wird.
    </p>
    <p>
      Dann kann man sich immer noch &#252;berlegen, ob man dann <i>an dieser Stelle</i>&#160;bereinigt
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1464387004423" FOLDED="true" ID="ID_1076928743" MODIFIED="1488423308085" TEXT="Fehlerbehandlung">
<node CREATED="1464387034539" FOLDED="true" ID="ID_1580200225" MODIFIED="1488423308063" TEXT="injectNew">
<icon BUILTIN="button_ok"/>
<node CREATED="1464387043146" ID="ID_1075443725" MODIFIED="1465045655821" TEXT="API erweitern">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1464387046769" ID="ID_1282279036" MODIFIED="1465045658691" TEXT="R&#xfc;ckgabewert">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1464387055632" ID="ID_381742708" MODIFIED="1465045661504" TEXT="andere Layer fragen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1464387071669" FOLDED="true" ID="ID_1152000090" MODIFIED="1488423308063" TEXT="skip">
<icon BUILTIN="button_cancel"/>
<node CREATED="1464387092091" ID="ID_534085817" MODIFIED="1464387095959" TEXT="hat kein Argument"/>
<node CREATED="1464387096498" ID="ID_1113073682" MODIFIED="1464387158085" TEXT="kann daher Selektor nicht ausf&#xfc;hren">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1464387102850" ID="ID_1022478410" MODIFIED="1464387148396" TEXT="Fehlererkennung nicht m&#xf6;glich">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1464386771270" FOLDED="true" ID="ID_1279882649" MODIFIED="1488423308085" TEXT="setter vs mutator">
<node CREATED="1464386870265" ID="ID_1326576761" MODIFIED="1464387168773" TEXT="Metaprogrammierung"/>
<node CREATED="1464386878983" ID="ID_19052291" MODIFIED="1464386887354" TEXT="Code selber mu&#xdf; es implizit wissen"/>
<node CREATED="1464386904676" FOLDED="true" ID="ID_1844234642" MODIFIED="1488423308064" TEXT="entweder-oder">
<icon BUILTIN="help"/>
<node CREATED="1464386910099" ID="ID_127180278" MODIFIED="1464386913750" TEXT="nicht notwendig"/>
<node CREATED="1464386929745" ID="ID_1833671444" MODIFIED="1464386936708" TEXT="es greift der Layer, der es kann"/>
</node>
<node CREATED="1464483482949" FOLDED="true" HGAP="32" ID="ID_597384200" MODIFIED="1488423308064" TEXT="Code-Struktur" VSHIFT="15">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1464483497524" ID="ID_206433048" MODIFIED="1464483503639" TEXT="Basisklasse + Overlay"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1464483504155" FOLDED="true" ID="ID_565058375" MODIFIED="1488423307136" TEXT="unsch&#xf6;n und verwirrend">
<node CREATED="1464483533863" ID="ID_1813082482" MODIFIED="1464483617082" TEXT="mehrfach geschachtelte Typdefinitionen">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1464483544429" ID="ID_467553331" MODIFIED="1464483659487" TEXT="Problem mit der EntryID">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      in einem Fall kann man sie aus der Closure abgreifen
    </p>
    <p>
      im anderen Fall mu&#223; es doch der Client leisten.
    </p>
    <p>
      Keine klare Linie
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1464483601318" ID="ID_1275035047" MODIFIED="1464483610226" TEXT="gibt es &#xfc;berhaupt eine Alternative">
<icon BUILTIN="help"/>
</node>
<node CREATED="1465041597680" FOLDED="true" ID="ID_31475692" MODIFIED="1488423307136" TEXT="Abw&#xe4;gung">
<node CREATED="1465040677452" ID="ID_1313751992" MODIFIED="1465040776226" TEXT="OK: Klassen sind geschnitten nach Belang">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t, es gibt nur minimale, themantische &#220;berlappung.
    </p>
    <p>
      Also ist die Verwendung von Vererbung hier sogar die <i>beste</i>&#160;L&#246;sung
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1465040780654" ID="ID_73964489" MODIFIED="1465040812722">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Geschachtelte Typdefs lassen sich vermeiden:
    </p>
    <p>
      BareEntryID speichern!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1465040890735" ID="ID_19731522" MODIFIED="1465041541616" TEXT="API f&#xfc;r die ID ist ein Dilemma">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t, wie rum man es auch aufl&#246;st, wird die L&#246;sung auf einer Seite schlechter
    </p>
    <ul>
      <li>
        wenn wir f&#252;r den Payload-Typ einen Typ-Parameter nehmen, bl&#228;hen wir den Standard-Fall (Setter) auf
      </li>
      <li>
        andererseits <i>ist es unbstreitbar einfach so,</i>&#160;da&#223; f&#252;r den Mutator-Builder die Typisierung komplett implizit ist, das mu&#223; die Closure mit sich&#160;selbst ausmachen, einfach indem in der Closure ein geschachtelter TreeMutator konstruiert wird, der eben mit diesem impliziten Kind-Typ umgehen kann.
      </li>
      <li>
        wenn wir stattdessen nur einen Key-String speichern, wird zum Einen die Match-Pr&#252;fung aufwendiger (Stringvergleich statt Vergleich von Hashes), und au&#223;erdem wird ein Typ-Mismatch nicht mehr auf der Ebene der Verb-Anwendung entdeckt und entsprechend gekennzeichnet, sondern wir hoffen, da&#223; es dann innerhalb der Closure zu einem Fehlzugriff auf die Payload der GenNode kommt. Noch schlimmer im Mutator-Fall, da sind wir dann schon im geschachtelten Scope und hoffen, da&#223; dann der eingeschachtelte Mutator irgendwo auf Widerspruch l&#228;uft.
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1465041611663" ID="ID_1925576169" MODIFIED="1465041702243" TEXT="man kann auf DSL-Ebene mehrere Alternativen anbieten">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...gedacht f&#252;r verschiedene UseCases.
    </p>
    <ul>
      <li>
        Fall 1: String-Key und der Typ mu&#223; irgendwie implizit/explizit gegeben sein
      </li>
      <li>
        Fall2: GenNodeID
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1465139405203" ID="ID_103737978" MODIFIED="1465139492329" TEXT="...hey, der Typ ist implizit klar! es ist immer Rec">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...die offensichtlichsten Dinge &#252;bersieht man nur zu leicht!!!!!
    </p>
    <p>
      Da es ein nested scope ist, ist es immer ein Objekt,
    </p>
    <p>
      also repr&#228;sentiert als Rec&lt;GenNode&gt;
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1457232746179" FOLDED="true" ID="ID_1793680066" MODIFIED="1488423308114" TEXT="Rec&lt;GenNode&gt;" VSHIFT="11">
<icon BUILTIN="full-4"/>
<node CREATED="1464305377785" FOLDED="true" ID="ID_396910166" MODIFIED="1488423308100" TEXT="Vor&#xfc;berlegungen">
<node CREATED="1464305417066" FOLDED="true" ID="ID_1059660516" MODIFIED="1488423308085" TEXT="w&#xfc;nschenswert">
<icon BUILTIN="help"/>
<node CREATED="1464305708891" FOLDED="true" ID="ID_1184917831" MODIFIED="1488423308064" TEXT="Vorteile">
<node CREATED="1464305431944" ID="ID_426248387" MODIFIED="1464305641440" TEXT="den RecordContentMutator loswerden"/>
<node CREATED="1464305660018" ID="ID_255650962" MODIFIED="1464305687458" TEXT="die speziellen Regeln f&#xfc;r Attribute / Kinder loswerden"/>
</node>
<node CREATED="1464305712115" FOLDED="true" ID="ID_1789861154" MODIFIED="1488423308064" TEXT="Nachteile">
<node CREATED="1464305722209" ID="ID_375643343" MODIFIED="1464305733108" TEXT="man braucht immer einen TreeMutator"/>
<node CREATED="1464305735032" ID="ID_800203572" MODIFIED="1464305745786" TEXT="das ist sehr komplexer Code"/>
<node CREATED="1464305756189" ID="ID_696233984" MODIFIED="1464305766735" TEXT="zus&#xe4;tzliche Indirektion / VTable"/>
<node CREATED="1464305800207" ID="ID_1296045436" MODIFIED="1464305818736" TEXT="das &quot;Backdoor&quot; in Record::Mutator bleibt bestehen"/>
</node>
<node CREATED="1464305842833" FOLDED="true" ID="ID_365028607" MODIFIED="1488423308064" TEXT="Abw&#xe4;gung">
<node CREATED="1464305847001" FOLDED="true" ID="ID_845708071" MODIFIED="1488423307143" TEXT="Aufwand">
<node CREATED="1464305850440" FOLDED="true" ID="ID_592160280" MODIFIED="1488423307143" TEXT="dedizierte Impl">
<node CREATED="1464305871806" ID="ID_1200803242" MODIFIED="1464305874249" TEXT="ein Stack"/>
<node CREATED="1464306044598" FOLDED="true" ID="ID_780490719" MODIFIED="1488423307143" TEXT="pro Stackframe">
<node CREATED="1464305896754" ID="ID_569905839" MODIFIED="1464305908965" TEXT="ref Rec::Mutator"/>
<node CREATED="1464305910520" FOLDED="true" ID="ID_1883294095" MODIFIED="1488423307142" TEXT="Puffer: RecordContentMutator">
<node CREATED="1464305921943" ID="ID_1412580517" MODIFIED="1464305927370" TEXT="Vector Attribute"/>
<node CREATED="1464305927910" ID="ID_1641635660" MODIFIED="1464305933817" TEXT="Vector Kinder"/>
<node CREATED="1464305989422" ID="ID_614946430" MODIFIED="1464305994825" TEXT="ein vector::iterator"/>
</node>
</node>
</node>
<node CREATED="1464305854040" FOLDED="true" ID="ID_1876525606" MODIFIED="1488423307143" TEXT="per TreeMutator">
<node CREATED="1464305876237" ID="ID_633615671" MODIFIED="1464305878376" TEXT="ein Stack"/>
<node CREATED="1464306061588" FOLDED="true" ID="ID_735374750" MODIFIED="1488423307143" TEXT="pro Stackframe">
<node CREATED="1472781610135" FOLDED="true" ID="ID_150002810" MODIFIED="1488423307143">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>zwei </i>Bindings
    </p>
  </body>
</html></richcontent>
<node CREATED="1472781728046" ID="ID_1910688935" MODIFIED="1472781744825" TEXT="ref Quell-Vector"/>
<node CREATED="1472781629036" ID="ID_1614778599" MODIFIED="1472781751975" TEXT="Puffer: Vector GenNode"/>
<node CREATED="1472781648571" ID="ID_1190592868" MODIFIED="1472781677753" TEXT="ein zugeh&#xf6;riger Iterator"/>
</node>
</node>
</node>
</node>
<node CREATED="1472781868035" FOLDED="true" HGAP="40" ID="ID_195135751" MODIFIED="1488423307143" TEXT="Unterschied minimal" VSHIFT="-3">
<node CREATED="1472781885352" ID="ID_327255109" MODIFIED="1472781888836" TEXT="zwei Iteratoren"/>
<node CREATED="1472781889328" ID="ID_1298906038" MODIFIED="1472781899051" TEXT="zwei Quell-Referenzen"/>
</node>
</node>
</node>
<node CREATED="1472653531806" FOLDED="true" HGAP="76" ID="ID_1466924103" MODIFIED="1488423308085" TEXT="Ansatz" VSHIFT="-10">
<node CREATED="1472653541019" ID="ID_1910363821" MODIFIED="1472653569512">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>zwei</i>&#160;Collection-Bindings
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1472653548905" ID="ID_56400821" MODIFIED="1472653553124" TEXT="speziell konfiguriert"/>
<node CREATED="1472654905416" ID="ID_729856704" MODIFIED="1472654910067" TEXT="minimaler Code"/>
</node>
</node>
<node COLOR="#338800" CREATED="1464305382975" FOLDED="true" ID="ID_1038582300" MODIFIED="1488423308100" TEXT="Implementierung">
<icon BUILTIN="button_ok"/>
<node CREATED="1457742036967" FOLDED="true" ID="ID_980619979" MODIFIED="1488423308085" TEXT="Attribute">
<node CREATED="1472781560508" ID="ID_1534877939" MODIFIED="1472781584718" TEXT="Selector: isNamed"/>
<node CREATED="1472930964666" ID="ID_1394229571" MODIFIED="1473014470867" TEXT="Behandlung f&#xfc;r Typ-Feld"/>
</node>
<node CREATED="1457742040782" ID="ID_613087606" MODIFIED="1457742042442" TEXT="Kinder"/>
<node CREATED="1472781922380" FOLDED="true" ID="ID_506316487" MODIFIED="1488423308085" TEXT="Problem: rekursiver Mutator">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1472781942185" ID="ID_882829469" MODIFIED="1472781946644" TEXT="rein code-technisches Problem"/>
<node CREATED="1472781957063" ID="ID_1617563820" MODIFIED="1472781964177" TEXT="durch forward-decl aufzul&#xf6;sen"/>
<node CREATED="1472781977156" ID="ID_1731364667" MODIFIED="1472782008196" TEXT="erfordert Umbau der Code-Struktur">
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node CREATED="1472930979111" FOLDED="true" ID="ID_1629990314" MODIFIED="1488423308085" TEXT="Problem: Typ-Feld">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1472931031568" FOLDED="true" ID="ID_1634653912" MODIFIED="1488423308064" TEXT="Kompromi&#xdf; in GenNode">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1472931061612" ID="ID_1482076461" MODIFIED="1472931066111" TEXT="halbherziges Design"/>
<node CREATED="1472931066635" ID="ID_1488685471" MODIFIED="1472931074166" TEXT="wollte nicht wirklich Metadaten einf&#xfc;hren"/>
<node CREATED="1472931074754" ID="ID_1078549727" MODIFIED="1472931083789" TEXT="hab&apos;s daher dann fest verdrahtet"/>
<node CREATED="1472931084281" ID="ID_358955160" MODIFIED="1472931095987" TEXT="erlaube &quot;Abk&#xfc;rzung&quot; &#xfc;ber das Typ-Attribut"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1472931096647" ID="ID_1730961697" MODIFIED="1472931185935" TEXT="aber nicht wirklich konsequent...">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...diese Abk&#252;rzung ist nur auf den Konstruktur aufgepflanzt,
    </p>
    <p>
      nicht aber in der eigentlichen Implementierung verankert.
    </p>
    <p>
      Das wollte ich nicht, weil ich l&#228;ngerfristig doch davon ausgehe,
    </p>
    <p>
      da&#223; es einfach einen Metadaten-Scope gibt
    </p>
    <p>
      
    </p>
    <p>
      Die Inkonsequenz nun ist, da&#223; im Rec::Mutator keine Magie daf&#252;r vorgesehen ist
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1472931195618" FOLDED="true" ID="ID_1075996120" MODIFIED="1488423308064" TEXT="aber Meta-Attribut f&#xfc;r Diff">
<node CREATED="1472931234165" ID="ID_160523779" MODIFIED="1472931239279" TEXT="geht nicht anders"/>
<node CREATED="1472931241027" ID="ID_1519973794" MODIFIED="1472931247263" TEXT="direkte Folge des Objekt-Modells"/>
<node CREATED="1472931391998" FOLDED="true" ID="ID_1361356376" MODIFIED="1488423307151" TEXT="betrifft">
<node CREATED="1472931395526" ID="ID_1159599973" MODIFIED="1472931403393" TEXT="ins(type = X)"/>
<node CREATED="1472931404525" FOLDED="true" ID="ID_1215961897" MODIFIED="1488423307151" TEXT="set(type = X)">
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1472931416323" ID="ID_1856124935" MODIFIED="1472931434577" TEXT="AUA: bisher &#xfc;bersehen">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
</node>
<node CREATED="1472931275575" FOLDED="true" ID="ID_158751268" MODIFIED="1488423308065" TEXT="Collection-Binding">
<node CREATED="1472931287229" ID="ID_1702269467" MODIFIED="1472931308630" TEXT="klinkt sich unterhalb vom Mutator ein"/>
<node CREATED="1472931309122" ID="ID_632255284" MODIFIED="1472931324740" TEXT="umgeht den Mutator"/>
<node CREATED="1472931326512" ID="ID_150253618" MODIFIED="1472931332867" TEXT="arbeitet direkt auf den Collections"/>
<node CREATED="1472931333975" ID="ID_330064662" MODIFIED="1472931342433" TEXT="reicht Werte unbetrachtet 1:1 durch"/>
</node>
<node CREATED="1472931711315" FOLDED="true" ID="ID_1747250133" MODIFIED="1488423308065" TEXT="L&#xf6;sung">
<icon BUILTIN="idea"/>
<node CREATED="1472931716482" ID="ID_1328626801" MODIFIED="1472931798657" TEXT="Lambdas nicht ausreichend">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...mit den Lambdas kann ich nur die Sicht auf die Werte steuern,
    </p>
    <p>
      nicht aber das eigentliche Verhalten des Bindings.
    </p>
    <p>
      
    </p>
    <p>
      Denn die Lambdas haben keinen Zugriff auf die Ziel-Datenstruktur!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1472931806958" ID="ID_321203237" MODIFIED="1472931822190" TEXT="m&#xfc;&#xdf;te Binding-Operationen anpassen">
<icon BUILTIN="smily_bad"/>
</node>
<node COLOR="#338800" CREATED="1472931829467" FOLDED="true" ID="ID_383672039" MODIFIED="1488423307152" STYLE="fork" TEXT="Dekorator">
<icon BUILTIN="button_ok"/>
<node CREATED="1472934618917" ID="ID_1010335466" MODIFIED="1472934744607" TEXT="legt sich um den &#xe4;u&#xdf;eren Collection-Binding-Layer"/>
<node CREATED="1472934635426" ID="ID_610715894" MODIFIED="1472934744607" TEXT="greift bei der injectNew und assignElm-Operation ein"/>
<node CREATED="1472934671013" ID="ID_1605900118" MODIFIED="1472934744607" TEXT="braucht Zugang zum Rec::Mutator">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1472934695226" ID="ID_247894372" MODIFIED="1472934744607" TEXT="Code wird schwer lesbar"/>
<node CREATED="1472934710032" FOLDED="true" ID="ID_1233852840" MODIFIED="1488423307151" TEXT="umformulieren">
<icon BUILTIN="button_ok"/>
<node CREATED="1473014547678" ID="ID_1566443628" MODIFIED="1473014763323" TEXT="geht nur bedingt">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1473014552677" ID="ID_1444718355" MODIFIED="1473014759258" TEXT="Problem ist der Builder">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wir wollen mehrfach geschichtete TreeMutator-Subklassen,
    </p>
    <p>
      aber tats&#228;chlich liefert jeder DSL-Aufruf einen Builder&lt;TreeMutator&lt;...&gt;&gt;.
    </p>
    <p>
      Die normalen DSL-Aufrufe sind eben genau so gestrickt, da&#223; jeweils der oberste Builder entfernt wird,
    </p>
    <p>
      ein neuer Layer dar&#252;bergebaut und das Ganze wieder in einen Builder eingewickelt wird.
    </p>
    <p>
      
    </p>
    <p>
      Dadurch ist es schwer bis unm&#246;glich (wg. den Lambdas), den resultierenden Typ anzuschreiben.
    </p>
    <p>
      Daher bin ich zwingend auf Wrapper-Funktionen angewiesen, die diesen resultierenden Typ
    </p>
    <p>
      vom konkreten Aufruf wieder &quot;abgreifen&quot;. Ich kann daher nicht die DSL-Notation verwenden,
    </p>
    <p>
      um den Dekorator f&#252;r die Behandlung des Typ-Feldes einzubringen.
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1472782025318" FOLDED="true" ID="ID_387350632" MODIFIED="1488423308085" TEXT="Standard-Lambdas f&#xfc;r GenNode">
<node CREATED="1472782055217" ID="ID_1053081307" MODIFIED="1472782069243" TEXT="Selector -&gt; ID-Vergleich"/>
<node CREATED="1472782069784" ID="ID_893616200" MODIFIED="1472782075874" TEXT="Constructor -&gt; copy"/>
<node CREATED="1472782077062" ID="ID_1012433129" MODIFIED="1472782092512" TEXT="Setter -&gt; assign DataCap"/>
<node CREATED="1472782093956" ID="ID_1200489627" MODIFIED="1472782120696">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Mut -&gt; <font color="#e43e2a">Rekursion</font>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1472830089042" FOLDED="true" ID="ID_1759032781" MODIFIED="1488423308086" TEXT="until after Ref::ATTRIBS">
<node CREATED="1472830107591" ID="ID_1479915312" MODIFIED="1472830118609" TEXT="mu&#xdf; Semantik im TreeMutator API korrigieren"/>
<node CREATED="1472830131284" ID="ID_1581714242" MODIFIED="1472830153132" TEXT="Problem: nur die Implementierung wei&#xdf;, was ein &quot;Attribut&quot; ist"/>
<node CREATED="1472830122757" ID="ID_937476321" MODIFIED="1472830129487" TEXT="brauche Unterst&#xfc;tzung vom Selector"/>
<node CREATED="1472830164119" FOLDED="true" ID="ID_1323583081" MODIFIED="1488423308065" TEXT="L&#xf6;sungsansatz">
<node CREATED="1472830177221" ID="ID_422155652" MODIFIED="1472830181729" TEXT="wie Ref::END"/>
<node CREATED="1472830182101" ID="ID_275162903" MODIFIED="1472830195511" TEXT="aber nur einige Layer reagieren darauf"/>
<node CREATED="1472830199203" FOLDED="true" ID="ID_35560751" MODIFIED="1488423307152" TEXT="default-return mu&#xdf; true sein">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1472830226455" ID="ID_1975724782" MODIFIED="1472830232658" TEXT="aber nat&#xfc;rlich nur wenn leer..."/>
<node CREATED="1472830286462" ID="ID_1170732286" MODIFIED="1472830298035" TEXT="Einsicht: END und ATTRIBS k&#xf6;nnen nicht scheitern">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1472830304844" ID="ID_354507274" MODIFIED="1472840380057" TEXT="...es geht nur um das Layer-Chaining"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node COLOR="#4a273f" CREATED="1461882151581" FOLDED="true" HGAP="29" ID="ID_710057839" MODIFIED="1488423342547" TEXT="Selector integrieren" VSHIFT="23">
<icon BUILTIN="button_ok"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1461967533959" ID="ID_1476036864" MODIFIED="1468761946715" TEXT="korrekte logische Verkettung"/>
<node CREATED="1461967541166" ID="ID_1033772766" MODIFIED="1468761946715" TEXT="mu&#xdf; von jeder Primitive unterst&#xfc;tzt werden">
<icon BUILTIN="messagebox_warning"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1461967571042" FOLDED="true" ID="ID_186074377" MODIFIED="1488423308114">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>Problem</b><font color="#4e094b">: partielle Ordnung</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1461967744747" ID="ID_1012577830" MODIFIED="1468761946715" TEXT="Diff setzt totale Ordnung voraus"/>
<node CREATED="1461967765080" ID="ID_1011855147" MODIFIED="1468761946715" TEXT="verletzt durch &#xbb;onion-layer&#xab;"/>
<node CREATED="1461967839110" FOLDED="true" ID="ID_571996411" MODIFIED="1488423308100" TEXT="Relevanz">
<icon BUILTIN="help"/>
<node CREATED="1461967850797" ID="ID_1981205783" MODIFIED="1468761946715" TEXT="bei Einzelschritt irrelevant"/>
<node CREATED="1461967876521" FOLDED="true" ID="ID_1804785019" MODIFIED="1488423308086" TEXT="zwingend bei &quot;AFTER&quot;-Verb">
<node CREATED="1461967962814" ID="ID_1667097505" MODIFIED="1472219338757" TEXT="weil das Zwischenschritte &#xfc;berspringt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t,
    </p>
    <p>
      das AFTER-Verb wird &#252;bersetzt in ein skip_until,
    </p>
    <p>
      und das l&#228;uft dann entweder in jedem Layer
    </p>
    <p>
      oder nur in dem Layer, der auf die Spec pa&#223;t.
    </p>
    <p>
      In jedem Fall ger&#228;t dadurch die relative Verzahnung der Elemente untereinander aus dem Takt
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1461967992042" ID="ID_9552950" MODIFIED="1468761946715" TEXT="nur ein Ausweg..."/>
<node CREATED="1461968004720" ID="ID_1333404241" MODIFIED="1472219338765" TEXT="Ordnung der onion-layer verbindlich machen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t also, es wird stets der zuerst gebundene Layer komplett durchgespult,
    </p>
    <p>
      gefolgt dann von dem n&#228;chsten Layer.
    </p>
    <p>
      
    </p>
    <p>
      Die Konsequenz ist, da&#223; es keine Mischung der Typen geben kann.
    </p>
    <p>
      Es m&#252;ssen immer zwingend alle Elemente eines Typs von einem Layer behandelt werden
    </p>
    <p>
      und diese Elemente m&#252;ssen geschlossen hintereinander in der Reihenfolge liegen
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1465428839332" FOLDED="true" ID="ID_909803930" MODIFIED="1488423342547" TEXT="Diff-Anwendung" VSHIFT="36">
<icon BUILTIN="button_ok"/>
<node CREATED="1465428850946" FOLDED="true" ID="ID_1085608480" MODIFIED="1488423308114">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      auf Basis des neu geschaffenen <b>TreeMutator</b>s
    </p>
  </body>
</html></richcontent>
<node CREATED="1465664755169" FOLDED="true" ID="ID_132124444" MODIFIED="1488423308100" TEXT="Modus der Anwendung">
<node CREATED="1465664762944" ID="ID_1254081914" MODIFIED="1465665479208" TEXT="DiffApplicator&lt;TreeMutator&gt;">
<icon BUILTIN="help"/>
</node>
<node CREATED="1465664777398" ID="ID_872861883" MODIFIED="1472219338777" TEXT="Automatismen sind denkbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....man k&#246;nnte sp&#228;ter geeignete Automatismen schaffen,
    </p>
    <p>
      die sich diesen TreeMutator beschaffen
    </p>
    <ul>
      <li>
        indem erkannt wird, da&#223; das eigentliche Zielobjekt ein bestimmtes API bietet
      </li>
      <li>
        indem andere relevante Eigenschaften des Zielobjekts erkannt werden
      </li>
    </ul>
  </body>
</html></richcontent>
<arrowlink COLOR="#994062" DESTINATION="ID_1139754084" ENDARROW="Default" ENDINCLINATION="1028;0;" ID="Arrow_ID_10673972" STARTARROW="None" STARTINCLINATION="978;42;"/>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1465665483456" FOLDED="true" ID="ID_1329468559" MODIFIED="1488423308086" TEXT="Problem: konkreter Mutator">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das so h&#228;ufig in C++ auftretende Problem:
    </p>
    <p>
      wie baue und verwalte ich eine konkrete Implementierung,
    </p>
    <p>
      ohne gleich ein ganzes Management-Framework einf&#252;hren zu m&#252;ssen.
    </p>
    <p>
      Letzten Endes lief&#160;&#160;das auch in diesem Fall auf inline-Storage hinaus...
    </p>
  </body>
</html></richcontent>
<node CREATED="1465665492727" ID="ID_743684748" MODIFIED="1465665500210" TEXT="mu&#xdf; vom Target gebaut werden"/>
<node CREATED="1465665500830" ID="ID_937213383" MODIFIED="1465665561681" TEXT="Typ nicht bekannt">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1465665511580" ID="ID_611347489" MODIFIED="1465665556325" TEXT="soll auf Stack top-Level ">
<icon BUILTIN="yes"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1465665579587" FOLDED="true" ID="ID_786565560" MODIFIED="1488423308065" TEXT="problematisch">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1465665594969" FOLDED="true" ID="ID_1072421347" MODIFIED="1488423307168" TEXT="unbekannte Gr&#xf6;&#xdf;e">
<node CREATED="1465665884698" ID="ID_418230667" MODIFIED="1465665893757" TEXT="default-Bufferg&#xf6;&#xdf;e vorlegen"/>
<node CREATED="1465665894273" ID="ID_970277186" MODIFIED="1465665901124" TEXT="Gr&#xf6;&#xdf;e proben und anpassen"/>
<node CREATED="1465665901640" ID="ID_650164559" MODIFIED="1465665920298" TEXT="Gr&#xf6;&#xdf;e f&#xfc;r &quot;bekannte&quot; Targets statisch hinterlegen"/>
<node CREATED="1465852237233" FOLDED="true" HGAP="27" ID="ID_699424989" MODIFIED="1488423307168" TEXT="Beschlu&#xdf;" VSHIFT="12">
<icon BUILTIN="yes"/>
<node CREATED="1465852248487" ID="ID_1402619250" MODIFIED="1465852255154" TEXT="zun&#xe4;chst die einfachste L&#xf6;sung"/>
<node CREATED="1465852255694" ID="ID_462293433" MODIFIED="1472122166677" TEXT="statisch (fest) festlegen"/>
<node CREATED="1465852268252" ID="ID_329653886" MODIFIED="1465852274743" TEXT="ggfs. noch nach Zieltyp differenziert"/>
<node CREATED="1465852275859" ID="ID_394082851" MODIFIED="1465852288510" TEXT="dynamisch / Lernen als M&#xf6;glichkeit vorsehen"/>
</node>
</node>
<node CREATED="1465665614990" FOLDED="true" ID="ID_1851089586" MODIFIED="1488423307169" TEXT="verwirrendes API">
<node CREATED="1465665980686" ID="ID_1537039073" MODIFIED="1465665991232" TEXT="Anwendung sollte selbstevident sein"/>
<node CREATED="1465666009690" FOLDED="true" ID="ID_1006664389" MODIFIED="1488423307169" TEXT="Typ-Signatur sollte den Weg weisen">
<node CREATED="1465674483754" ID="ID_155190959" MODIFIED="1465674558135" TEXT="sehe nur eine L&#xf6;sung">
<icon BUILTIN="forward"/>
</node>
<node CREATED="1465674488874" ID="ID_625451231" MODIFIED="1465674563848" TEXT="Mutator-Builder-Funktion">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1465674529460" ID="ID_845292312" MODIFIED="1465674571112" TEXT="pflanzt in ein gegebenes Buff-Handle">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1465665993012" ID="ID_1531461875" MODIFIED="1465666003750" TEXT="keine komplexen &quot;Riten&quot;"/>
<node CREATED="1465666524573" FOLDED="true" HGAP="30" ID="ID_462712191" MODIFIED="1488423307169" TEXT="Schlu&#xdf;folgerung" VSHIFT="18">
<icon BUILTIN="yes"/>
<node CREATED="1465666541419" ID="ID_1090547868" MODIFIED="1465666552293" TEXT="entweder ein festes Interface"/>
<node CREATED="1465666552842" ID="ID_1518882587" MODIFIED="1465666574594" TEXT="oder &#xfc;ber Metaprogrammierung / Spezialisierung"/>
<node CREATED="1468761997202" FOLDED="true" ID="ID_1870085622" MODIFIED="1488423307169" TEXT="kombinierte L&#xf6;sung">
<icon BUILTIN="idea"/>
<node CREATED="1468762009304" ID="ID_1554371522" MODIFIED="1468762018314" TEXT="Interface DiffMutable"/>
<node CREATED="1468762031701" FOLDED="true" ID="ID_48737788" MODIFIED="1488423307169" TEXT="Meta-Adapter">
<linktarget COLOR="#503382" DESTINATION="ID_48737788" ENDARROW="Default" ENDINCLINATION="8;131;" ID="Arrow_ID_579805576" SOURCE="ID_887400042" STARTARROW="None" STARTINCLINATION="535;0;"/>
<node CREATED="1468762052858" ID="ID_494010261" MODIFIED="1468762066764" TEXT="synthetisiert dieses Interface"/>
<node CREATED="1468762067383" ID="ID_484867304" MODIFIED="1468762075059" TEXT="erkennt passende Methoden"/>
<node CREATED="1468762075671" FOLDED="true" ID="ID_52899390" MODIFIED="1488423307169" TEXT="Problem: Storage">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1468762092660" ID="ID_361660092" MODIFIED="1468762097264" TEXT="Adapter oder Lambda"/>
<node CREATED="1468762097684" ID="ID_1049198962" MODIFIED="1468762107374" TEXT="mu&#xdf; l&#xe4;nger leben als der Adapter"/>
<node CREATED="1468762118913" ID="ID_1064755787" MODIFIED="1468762128867" TEXT="L&#xf6;sung: Wert / Reftyp per Spezialisierung"/>
<node CREATED="1468762138318" ID="ID_1261183393" MODIFIED="1472219338878" TEXT="Restrisiko bleibt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....da&#223; ein unbedarfter client diesen Trick &#252;bershieht
    </p>
    <p>
      und daher den R&#252;ckgabewert wegwirft.
    </p>
    <p>
      
    </p>
    <p>
      Argument: we soweit einsteigt, die Metaprogramming-L&#246;sung zu nutzen,
    </p>
    <p>
      sollte auch intelligent genug sein, die API-Doc zu lesen.
    </p>
    <p>
      
    </p>
    <p>
      Standard == Interface DiffMutable implementieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1469121877371" FOLDED="true" HGAP="38" ID="ID_1718572301" MODIFIED="1488423307169" TEXT="freie funktion" VSHIFT="16">
<icon BUILTIN="yes"/>
<node CREATED="1469121882782" ID="ID_1873664189" MODIFIED="1469121933463" TEXT="keine Spezialisierung notwendig">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1469121890060" ID="ID_985582322" MODIFIED="1469121930007">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Client soll direkt <b>mutatorBinding</b>&#160;bieten
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1469544027620" FOLDED="true" HGAP="38" ID="ID_1475348824" MODIFIED="1488423307170" TEXT="Storage-Probleme" VSHIFT="11">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1469544055624" FOLDED="true" ID="ID_390940604" MODIFIED="1472140990090" TEXT="Beobachtung: Template-bloat">
<icon BUILTIN="stop-sign"/>
<node CREATED="1469544072709" ID="ID_16055877" MODIFIED="1469544083016" TEXT="Ursache: wir generieren die Klasse jedesmal neu"/>
<node CREATED="1469544084220" ID="ID_1247343007" MODIFIED="1469544104909" TEXT="...obwohl keinerlei Bezug zum Target-Typ vorliegt">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1469544123271" ID="ID_1703312587" MODIFIED="1469544133675" TEXT="Funktionalit&#xe4;t ist zu 90% generisch">
<icon BUILTIN="info"/>
</node>
<node CREATED="1469544171176" ID="ID_887400042" MODIFIED="1469544253263">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>nicht</i>&#160;generisch: <b><font color="#923977">mutatorBinding</font></b>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#503382" DESTINATION="ID_48737788" ENDARROW="Default" ENDINCLINATION="8;131;" ID="Arrow_ID_579805576" STARTARROW="None" STARTINCLINATION="535;0;"/>
</node>
</node>
<node CREATED="1469544264955" FOLDED="true" HGAP="35" ID="ID_366424331" MODIFIED="1472140986107" VSHIFT="16">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      L&#246;sungsversuch: <b>extern template</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
<node CREATED="1469544293574" ID="ID_947583528" MODIFIED="1469544318455" TEXT="Template bereits in Lumiera-Lib vor-generieren">
<icon BUILTIN="info"/>
</node>
<node CREATED="1469544337408" ID="ID_1646108569" MODIFIED="1469544349252" TEXT="nur ctor ist nochmals ge-Templated">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1469544323331" FOLDED="true" ID="ID_1924059831" MODIFIED="1488423307169" TEXT="flexibler Teil verbleibt in ctor">
<icon BUILTIN="forward"/>
<node CREATED="1469544368180" ID="ID_989521105" MODIFIED="1469544377687" TEXT="hier kann das mutatorBinding stattfinden"/>
<node CREATED="1469544385882" ID="ID_82786122" MODIFIED="1469544397396" TEXT="...in Abh&#xe4;ngigkeit vom konkreten Zieltyp"/>
<node CREATED="1469544400544" ID="ID_1789924824" MODIFIED="1469544413058" TEXT="Resultat ist ein TreeMutator im internen Puffer"/>
</node>
<node CREATED="1469544471294" FOLDED="true" ID="ID_1306807662" MODIFIED="1488423307169" TEXT="Problem-1: Puffer-Gr&#xf6;&#xdf;e">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1469544519439" ID="ID_1173588510" MODIFIED="1469544535057" TEXT="Plan: Heuristik + Traits"/>
<node CREATED="1469544536309" ID="ID_579318822" MODIFIED="1469544546951" TEXT="h&#xe4;ngt von konkretem Zieltyp ab"/>
<node CREATED="1469544547731" ID="ID_1296389584" MODIFIED="1469544596453" TEXT="kann nicht im generischen Teil stecken">
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1469544575808" ID="ID_259226539" MODIFIED="1469544599880" TEXT="Problem: vom generischen Teil ansprechen">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1469544601444" ID="ID_1593190605" MODIFIED="1469544616515" TEXT="L&#xf6;sung: weitere Indirektion">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1469544618738" ID="ID_324293110" MODIFIED="1472122283229" TEXT="vertretbar, da nur beim Scope-Wechsel....">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...im Klartext: diesen Zugriff von der generischen Implementierung
    </p>
    <p>
      auf den eingebauten Stack-Mechanismus ben&#246;tigen wir nur...
    </p>
    <ul>
      <li>
        einmal zu Beginn, bei der Konstruktion
      </li>
      <li>
        wenn wir in einen geschachtelten Scope eintreten
      </li>
      <li>
        wenn wir einen Solchen verlassen
      </li>
    </ul>
    <p>
      Zwar sind indirekte Calls aufwendiger, aber letzten Endes auch wieder nicht soooo aufwendig,
    </p>
    <p>
      da&#223; sie uns im gegebenen Kontext umbringen...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1469544496907" FOLDED="true" ID="ID_447935464" MODIFIED="1488423307170" TEXT="Problem-2: re-Initialisierung">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1469544831244" ID="ID_756911810" MODIFIED="1469544838662" TEXT="bestehendes Protokoll">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1469544839795" ID="ID_1308746305" MODIFIED="1469544858420" TEXT="Diff-Applikator kan beliebige Folge von Diffs akzeptieren"/>
<node CREATED="1469544859392" ID="ID_325998286" MODIFIED="1469544964778">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>intern:</i>&#160;eingebaute <b>initDiffApplication()</b>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wird automatisch vor Konsumieren eines Diff aufgerufen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1469544965890" ID="ID_443152890" MODIFIED="1469545023179">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>Widerspruch:</i>&#160;TreeMutator ist <b>Wegwerf</b>-Objekt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="broken-line"/>
</node>
<node COLOR="#f61b01" CREATED="1469545033584" ID="ID_578930155" MODIFIED="1469545058652" TEXT="Abbruch">
<icon BUILTIN="button_cancel"/>
<icon BUILTIN="stop-sign"/>
</node>
</node>
</node>
<node CREATED="1469545080826" FOLDED="true" HGAP="41" ID="ID_268789302" MODIFIED="1472140983547" VSHIFT="17">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      L&#246;sungsversuch: <b>doppelte H&#252;lle</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1469545168798" FOLDED="true" ID="ID_511952630" MODIFIED="1488423307170" TEXT="Architektur">
<node CREATED="1469545173893" ID="ID_1815808987" MODIFIED="1469545190599" TEXT="opaque: Kern == TreeMutator"/>
<node CREATED="1469545191899" ID="ID_1298718461" MODIFIED="1469545206853" TEXT="generisch: TreeMutator-Bindung"/>
<node CREATED="1469545208440" ID="ID_796164740" MODIFIED="1469545246870" TEXT="dediziert: Typ-Adapter"/>
</node>
<node CREATED="1469545401582" FOLDED="true" ID="ID_1145081121" MODIFIED="1488423307170" TEXT="konkrete Folgen...">
<node CREATED="1469545417579" ID="ID_474776082" MODIFIED="1469545427486" TEXT="alle virtuellen Methoden im generischen Teil"/>
<node CREATED="1469545428289" FOLDED="true" ID="ID_1077900797" MODIFIED="1488423307170" TEXT="generischer Teil verwendet nur Interfaces">
<node CREATED="1469545452406" ID="ID_369878659" MODIFIED="1469545455778" TEXT="TreeMutator"/>
<node CREATED="1469545456549" ID="ID_676404790" MODIFIED="1469545459401" TEXT="ScopeManager"/>
</node>
<node CREATED="1469545522445" FOLDED="true" ID="ID_1752356251" MODIFIED="1488423307170" TEXT="ScopeManager">
<node CREATED="1469545526380" ID="ID_141570709" MODIFIED="1469545535550" TEXT="Imp. wird per Zieltyp generiert"/>
<node CREATED="1469545536139" ID="ID_423387515" MODIFIED="1469545543750" TEXT="lebt in opaquem Buffer"/>
<node CREATED="1469545546209" ID="ID_229490952" MODIFIED="1469545561067" TEXT="enth&#xe4;lt Stack,"/>
<node CREATED="1469545562151" ID="ID_1390916085" MODIFIED="1469545568130" TEXT="mit passender Puffergr&#xf6;&#xdf;e"/>
</node>
<node CREATED="1469545592171" FOLDED="true" ID="ID_1826413238" MODIFIED="1488423307170" TEXT="dedizierter Applikator">
<icon BUILTIN="idea"/>
<node CREATED="1469545598922" ID="ID_815414440" MODIFIED="1469545609733" TEXT="h&#xe4;llt Referenz auf Zielobjekt"/>
<node CREATED="1469545611177" ID="ID_410214862" MODIFIED="1469545670672">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann daher <b>TreeMutator</b>&#160;konstruieren
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1469545641644" ID="ID_1811640827" MODIFIED="1469545662506">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und zwar per <b>mutatorBinding</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1469545691349" ID="ID_239269579" MODIFIED="1469545705980">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      implementiert somit <b>initDiffApplication()</b>
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1469545778873" FOLDED="true" HGAP="37" ID="ID_263708712" MODIFIED="1488423307170" VSHIFT="53">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      TODO: <font color="#010101">Namensgebung</font>
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#7c4b8e" DESTINATION="ID_263708712" ENDARROW="Default" ENDINCLINATION="-438;877;" ID="Arrow_ID_774958535" SOURCE="ID_660087752" STARTARROW="Default" STARTINCLINATION="2134;135;"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1469545959160" ID="ID_826999251" MODIFIED="1469545961076" TEXT="DiffApplicationStrategy&lt;DiffMutable&gt;"/>
<node CREATED="1469545962192" ID="ID_1539721850" MODIFIED="1469545971051" TEXT="feste explizite Spezialisierung"/>
<node CREATED="1469545972470" ID="ID_996523023" MODIFIED="1469545983800" TEXT="in non-Template-Basisklasse verwandeln"/>
<node CREATED="1469545990956" ID="ID_543764775" MODIFIED="1469546010389" TEXT="oder zumindest in Trait-Template + enable_if"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1465855369239" FOLDED="true" ID="ID_1875145732" MODIFIED="1488423308100" STYLE="fork" TEXT="Architektur">
<icon BUILTIN="button_ok"/>
<node CREATED="1465855402346" FOLDED="true" ID="ID_390477158" MODIFIED="1488423308086" TEXT="Grundgedanke">
<node CREATED="1465855407129" ID="ID_1269101170" MODIFIED="1472219324297" TEXT="Trennung von DiffInterpreter und TreeMutator"/>
<node CREATED="1465855418880" ID="ID_118862910" MODIFIED="1472219324297" TEXT="ersterer wird auf Basis von letzterem implementiert"/>
<node CREATED="1465855507380" ID="ID_63018388" MODIFIED="1472219324297">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      TreeMutator-<i>Binding</i>&#160;mu&#223; opaque bleiben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1465855561877" FOLDED="true" ID="ID_1466552960" MODIFIED="1488423308065">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Buffer-Gr&#246;&#223;en-Management <i>vorsehen</i>
    </p>
  </body>
</html></richcontent>
<node CREATED="1465855596992" ID="ID_1925577989" MODIFIED="1472219324298" TEXT="passiert im Scope des Ctors"/>
<node CREATED="1465855605879" ID="ID_961922886" MODIFIED="1472219324298" TEXT="bzw. im Scope des MUT-Verbs"/>
<node CREATED="1465855641898" ID="ID_1420998207" MODIFIED="1472219338902" TEXT="Heuristik + Versuch&amp;Irrtum">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t
    </p>
    <ul>
      <li>
        ein sinnvoller Startwert wird heuristisch vorgegeben
      </li>
      <li>
        wenn die Allokation scheitert, die Exception fangen und die tats&#228;chlich ben&#246;tigte Gr&#246;&#223;e merken
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1465855653265" ID="ID_1093267368" MODIFIED="1472219324298" TEXT="konfigurieren und lernen"/>
</node>
<node CREATED="1465855576187" ID="ID_1820859855" MODIFIED="1472219324298" TEXT="zun&#xe4;chst jedoch primitiv implementieren"/>
</node>
<node CREATED="1465856069657" FOLDED="true" ID="ID_1074974681" MODIFIED="1488423308086" TEXT="Struktur">
<node CREATED="1465856074888" ID="ID_63334235" MODIFIED="1472219324298" TEXT="TreeMutator == PImpl"/>
<node CREATED="1465856092638" ID="ID_587610953" MODIFIED="1472219324298" TEXT="BufferManager : opaque aber inline"/>
</node>
</node>
<node CREATED="1465864106739" FOLDED="true" ID="ID_1139754084" MODIFIED="1488423308101" TEXT="Zugang zum Mutator-Binding">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t:
    </p>
    <p>
      gegeben ein syntaktisch sinnvoller top-level-Aufruf (&quot;wende das Diff an&quot;)
    </p>
    <p>
      -- wie bzw. von wem bekommen wir dann ein Binding, das einen passenden TreeMutator konstruiert?
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#994062" DESTINATION="ID_1139754084" ENDARROW="Default" ENDINCLINATION="1028;0;" ID="Arrow_ID_10673972" SOURCE="ID_872861883" STARTARROW="None" STARTINCLINATION="978;42;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1465864167187" ID="ID_1126324374" MODIFIED="1465864171974" TEXT="Aufruf: freie Funktion"/>
<node CREATED="1465864172514" ID="ID_523639016" MODIFIED="1465864180668" TEXT="delegiert auf Template"/>
<node CREATED="1465864181193" ID="ID_214760095" MODIFIED="1465864192611" TEXT="explizite / regelbasierte Spezialisierung"/>
<node CREATED="1465864193607" FOLDED="true" ID="ID_910823091" MODIFIED="1488423308086" TEXT="Problem: Storage f&#xfc;r die Closure">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1465864217628" ID="ID_234805162" MODIFIED="1465864228510" TEXT="typischerweise sind alles transiente Objekte"/>
<node CREATED="1465864229170" ID="ID_1100042937" MODIFIED="1465864240501" TEXT="also mu&#xdf; die Closure als Wert-Objekt rausgegeben werden"/>
<node CREATED="1472495487478" FOLDED="true" ID="ID_57266631" MODIFIED="1488423308065" TEXT="L&#xf6;sung...">
<node CREATED="1472495496574" ID="ID_618531871" MODIFIED="1472495510056" TEXT="auto typed return value"/>
<node CREATED="1472495510531" ID="ID_1366125392" MODIFIED="1472495513703" TEXT="keep in local scope"/>
<node CREATED="1472495514203" ID="ID_1694911368" MODIFIED="1472495529197" TEXT="tightly integrate into ctor of DiffAplicator"/>
</node>
<node CREATED="1465864247152" FOLDED="true" ID="ID_1684145985" MODIFIED="1488423308065" TEXT="gesucht: elegantes API">
<node CREATED="1472494557762" ID="ID_1916007342" MODIFIED="1472494575309" TEXT="DiffAplicator&lt;TargetTyp&gt;">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1472494413960" ID="ID_1460674703" MODIFIED="1472494571740" TEXT="Erweiterungspunkt f&#xfc;r ADL..."/>
<node CREATED="1472494432219" ID="ID_1801329518" MODIFIED="1472494446981" TEXT="...wird vom ctor des Diff-Applikators verwendet"/>
<node CREATED="1472494466926" ID="ID_1882750560" MODIFIED="1472494484296" TEXT="...und der wiederum greift nur"/>
<node CREATED="1472494485388" ID="ID_1744987934" MODIFIED="1472494501141" TEXT="...wenn TreeDiffTraits definiert sind"/>
<node CREATED="1472494501778" ID="ID_300920526" MODIFIED="1472494506323" TEXT="not too bad">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
</node>
</node>
<node CREATED="1465860736044" FOLDED="true" ID="ID_40337378" MODIFIED="1488423308101" TEXT="Implementierung">
<icon BUILTIN="button_ok"/>
<node CREATED="1472121965308" ID="ID_292673963" MODIFIED="1472121974646" TEXT="Verb delegiert an Mutator-Primitive"/>
<node CREATED="1472121976018" ID="ID_421028989" MODIFIED="1472121995995" TEXT="Fehlerbehandlung anhand R&#xfc;ckgabewert dieser"/>
<node CREATED="1472122015157" ID="ID_151735071" MODIFIED="1472122038334" TEXT="sinnvolle interne Fehler-Funktionen aufbauen"/>
<node CREATED="1472141026525" FOLDED="true" ID="ID_671819323" MODIFIED="1488423308086" TEXT="Scope-Wechsel">
<icon BUILTIN="button_ok"/>
<node CREATED="1472172154624" FOLDED="true" ID="ID_602990638" MODIFIED="1488423308066" TEXT="Zugang zum erzeugten Mutator....">
<node CREATED="1472172310187" ID="ID_148408930" MODIFIED="1472172362034" TEXT="via buffer-Handle">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erscheint mir die am wenigsten &#252;berraschende L&#246;sung.
    </p>
    <p>
      und zwar per handle.get()
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1472172348526" FOLDED="true" ID="ID_1671229453" MODIFIED="1488423307175" TEXT="m&#xf6;gliche Alternativen">
<node CREATED="1472172364204" ID="ID_261496162" MODIFIED="1472172467939" TEXT="Closure gibt TreeMutator&amp; zur&#xfc;ck">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erscheint mir fehleranf&#228;llig und irref&#252;hrend f&#252;r den Nutzer der Schnittstelle.
    </p>
    <p>
      Denn er mu&#223; zwar das Objekt in das Handle platzieren, dann aber auch noch einen Pointer zur&#252;ckgeben,
    </p>
    <p>
      der dann auch noch NULL sein kann, zum Signalisieren von Fehlern.
    </p>
    <p>
      Ich empfinde das als schlechten Stil
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1472172467934" ID="ID_1258020411" MODIFIED="1472172562992" TEXT="ScopeManager-Interface erweitern">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      naja, das w&#228;re billig, aber auch wieder beliebig.
    </p>
    <p>
      Es macht keinen Sinn vom API-Design her, sondern man m&#252;&#223;te es halt machen,
    </p>
    <p>
      weil die Implementierung den Zeiger auf den geschachtelen sub-Mutator umsetzen mu&#223;.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
</node>
</node>
<node CREATED="1472172586703" FOLDED="true" ID="ID_608960091" MODIFIED="1488423308066" TEXT="pr&#xfc;fen">
<node CREATED="1472172593710" ID="ID_1915451499" MODIFIED="1472172608024" TEXT="geschachtelter Scope mu&#xdf; beim Verlassen komplett abgearbeitet sein"/>
<node CREATED="1472172608636" ID="ID_1263541514" MODIFIED="1472172634020" TEXT="wir steigen niemals &#xfc;ber den root-Scope hinaus auf"/>
</node>
<node CREATED="1472172651974" FOLDED="true" ID="ID_807127372" MODIFIED="1488423308066" TEXT="ins-Problem">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1472172658349" ID="ID_997769381" MODIFIED="1472172806077">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die Diff-Sprache verlangt,
    </p>
    <p>
      da&#223; vor dem &#214;ffnen des geschachtelten Scopes
    </p>
    <p>
      dieser zumindest einmal per ins &quot;angelegt&quot; wurde.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1472172711846" ID="ID_146912736" MODIFIED="1472172809794" TEXT="wir haben es hier mit objektwertigen Attributen zu tun">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1472172723853" ID="ID_1560317945" MODIFIED="1472172732503" TEXT="das ist ein sonderbarer Grenzfall"/>
<node CREATED="1472172733307" ID="ID_26853749" MODIFIED="1472172753564" TEXT="man wird die normalerweise auf Impl-Ebene erzeugen"/>
<node CREATED="1472172754136" ID="ID_297078058" MODIFIED="1472172765458" TEXT="ein INS hat dann nichts sinnvolles zu tun"/>
<node CREATED="1472172772494" ID="ID_337827865" MODIFIED="1472172795138" TEXT="default-Implementierung absorbiert das INS stillschweigend">
<icon BUILTIN="forward"/>
</node>
</node>
</node>
</node>
<node CREATED="1465860740971" FOLDED="true" ID="ID_1662787747" MODIFIED="1488423308101" TEXT="Unit-Test">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_1662787747" ENDARROW="Default" ENDINCLINATION="114;0;" ID="Arrow_ID_1608349913" SOURCE="ID_1209021141" STARTARROW="None" STARTINCLINATION="114;0;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1472122365079" FOLDED="true" ID="ID_1033283113" MODIFIED="1488423308086" TEXT="Aussagekr&#xe4;ftiges Beispiel-Diff">
<node CREATED="1472122379548" ID="ID_270981817" MODIFIED="1472122384864" TEXT="sollte alle Features vorf&#xfc;hren"/>
<node CREATED="1472122385380" ID="ID_773660909" MODIFIED="1472122391943" TEXT="sollte wichtige Grenzf&#xe4;lle aktivieren"/>
<node CREATED="1472122394387" ID="ID_1262619176" MODIFIED="1472122409477" TEXT="sollte keine sinnlosen Operationen verlangen"/>
</node>
<node CREATED="1472122416040" ID="ID_644657664" MODIFIED="1472122576120" TEXT="Analog zu den Primitiven in TreeDiffBinding_test">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das ist ein Versuch, den Code f&#252;r den Leser verst&#228;ndlich zu halten.
    </p>
    <p>
      Die Idee ist, da&#223; es einen high-level Unit-Test gibt, der die gesamte Diff-Anwendung durchspielt
    </p>
    <p>
      und dazu passend einen low-level Unit-Test, der analog die gleichen Operationen macht,
    </p>
    <p>
      allerdings direkt auf dem TreeDiff-Interface durch Aufruf der passenden Primitiv-Operaionen.
    </p>
    <p>
      Letztere m&#252;ssen f&#252;r jede Art von &quot;onion-layer&quot; (konkretes Binding) erneut implementiert
    </p>
    <p>
      und daher auch jeweils eigens per Unit-Test abgedeckt werden.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1472122581698" ID="ID_1932054966" MODIFIED="1472122688070" TEXT="sehr komplexes Binding auf privaten Datentyp">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das ist hier sinnvoll. Das Binding sollte komplexer sein,
    </p>
    <p>
      als in der Praxis auftretende Bindings. Warum? Weil letztere immer etwas einseitg sind
    </p>
    <p>
      und damit Abk&#252;rzungen im Code-Pfad ausn&#252;tzen. Die Gefahr schlummert aber im Zusammenspiel
    </p>
    <p>
      der konkreten Bindings mit mehreren &quot;onion layers&quot;!
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1465428871136" FOLDED="true" ID="ID_1015843161" MODIFIED="1488423308114" TEXT="integration mehrerer Bindungs-Layer">
<node CREATED="1469918339329" ID="ID_1427674245" MODIFIED="1469918349660" TEXT="DiffVirtualisedApplication_test"/>
<node CREATED="1469918356432" ID="ID_55400098" MODIFIED="1469918363427" TEXT="Selector korrekt integrieren">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1465428882150" FOLDED="true" ID="ID_1889489833" MODIFIED="1488423308114" TEXT="komplexer Integrationstest">
<node CREATED="1469918339329" ID="ID_1209021141" MODIFIED="1472219629782" TEXT="DiffVirtualisedApplication_test">
<arrowlink DESTINATION="ID_1662787747" ENDARROW="Default" ENDINCLINATION="114;0;" ID="Arrow_ID_1608349913" STARTARROW="None" STARTINCLINATION="114;0;"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1458850101524" FOLDED="true" HGAP="-22" ID="ID_780586600" MODIFIED="1491009348902" TEXT="TODO" VSHIFT="29">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1458850122091" FOLDED="true" ID="ID_15024219" MODIFIED="1488423308114" TEXT="Collection-Binding: bessere Fehlermeldungen">
<node CREATED="1458850141920" ID="ID_428365633" MODIFIED="1472498706370" TEXT="bei Signatur-Mismatch">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn es ist sehr verwirrend, welche Signatur denn nun die Lambdas haben m&#252;ssen
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#a9b4c1" DESTINATION="ID_428365633" ENDARROW="Default" ENDINCLINATION="176;0;" ID="Arrow_ID_1501436647" SOURCE="ID_338749301" STARTARROW="None" STARTINCLINATION="176;0;"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1458850154575" ID="ID_725958518" MODIFIED="1472219338982" TEXT="wenn der Default-Matcher greift">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn es kann keinen Default-Matcher geben....
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1458850212503" ID="ID_321552356" MODIFIED="1472498583464" TEXT="Signaturen f&#xfc;r Lambdas dokumentieren">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1465860661877" ID="ID_942624903" MODIFIED="1472498127725" TEXT="Builder-API f&#xfc;r Attribut-Binding dokumentieren">
<icon BUILTIN="button_ok"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1465860672852" FOLDED="true" ID="ID_432641404" MODIFIED="1488423308114" TEXT="Variante zum Attribut-Binding, wobei die Attribut-ID als GenNode::ID gegeben ist">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1472498724880" ID="ID_1122881728" MODIFIED="1472498733392" TEXT="f&#xfc;r sp&#xe4;ter">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1472498734359" ID="ID_888090802" MODIFIED="1472498742262" TEXT="brauchen wir das wirklich">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1460847222865" ID="ID_362820787" MODIFIED="1472219338993" TEXT="abstrakte operationale Semantik der Diff-Anwendung dokumentieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...sonst wird niemand Lambdas bereitstellen k&#246;nnen, oder gar Diff-Nachrichten erzeugen.
    </p>
    <p>
      Das ist nun kein spezielles Problem der gew&#228;hlten Implementierungs-Technik, sondern r&#252;hrt daher,
    </p>
    <p>
      da&#223; der Client hier eigentlich ein Protokoll implementieren mu&#223;.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1458850263688" FOLDED="true" ID="ID_555018476" MODIFIED="1488423308114" TEXT="Zusammenf&#xfc;hren">
<node CREATED="1458850294083" ID="ID_393704697" MODIFIED="1458850302901" TEXT="der normale GenNode-Applikator"/>
<node CREATED="1458850282285" FOLDED="true" ID="ID_435722140" MODIFIED="1488423308101" TEXT="Pr&#xfc;fen ob sinnvoll">
<node CREATED="1458850328775" ID="ID_1736970011" MODIFIED="1458850333475" TEXT="Indirektionen"/>
<node CREATED="1458850334110" ID="ID_1313695007" MODIFIED="1458850338898" TEXT="Speicherbedarf"/>
<node CREATED="1458850339413" ID="ID_476682196" MODIFIED="1458850359702" TEXT="Aufwand bei der Verwendung"/>
</node>
<node CREATED="1458850306506" ID="ID_425272158" MODIFIED="1458850323212" TEXT="durch Binding f&#xfc;r Tree-Applikator ersetzen"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1461968295826" FOLDED="true" ID="ID_693317592" MODIFIED="1488423308115" TEXT="pr&#xfc;fen: soll das &#xbb;AFTER&#xab;-Verb aus der Sprache entfernt werden">
<linktarget COLOR="#592379" DESTINATION="ID_693317592" ENDARROW="Default" ENDINCLINATION="577;0;" ID="Arrow_ID_1115282938" SOURCE="ID_1922580473" STARTARROW="Default" STARTINCLINATION="1205;231;"/>
<icon BUILTIN="help"/>
<node CREATED="1461968428712" ID="ID_565746001" MODIFIED="1461968442674" TEXT="es ist n&#xfc;tzlich f&#xfc;r handgeschriegbene Diffs"/>
<node CREATED="1461968443334" ID="ID_1266765273" MODIFIED="1461968454521" TEXT="es vertr&#xe4;gt sich nicht mit gemischten Kindern"/>
<node CREATED="1465428190234" FOLDED="true" ID="ID_1957591180" MODIFIED="1488423308101" TEXT="Kompromi&#xdf;">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_1957591180" ENDARROW="Default" ENDINCLINATION="-27;14;" ID="Arrow_ID_228215960" SOURCE="ID_310892577" STARTARROW="None" STARTINCLINATION="139;3;"/>
<icon BUILTIN="idea"/>
<node CREATED="1465428196730" ID="ID_117877138" MODIFIED="1465428204548" TEXT="belassen"/>
<node CREATED="1465428205352" ID="ID_887510195" MODIFIED="1465428215635" TEXT="akzeptieren, da&#xdf; es nicht immer anwendbar ist"/>
<node CREATED="1465428216271" ID="ID_1296901581" MODIFIED="1465428234145" TEXT="die generische Variante &quot;after Ref::END&quot; ist sehr gut"/>
<node CREATED="1465428236860" ID="ID_339833424" MODIFIED="1465428248614" TEXT="speziell Attribut-Binding unterst&#xfc;tzt zur generische Variante"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1469545751885" ID="ID_660087752" MODIFIED="1469545906061" TEXT="Namensgebung f&#xfc;r generisches Mutator-Binding">
<arrowlink COLOR="#7c4b8e" DESTINATION="ID_263708712" ENDARROW="Default" ENDINCLINATION="-438;877;" ID="Arrow_ID_774958535" STARTARROW="Default" STARTINCLINATION="2134;135;"/>
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1472216217388" FOLDED="true" HGAP="21" ID="ID_1810727202" MODIFIED="1488423308115" TEXT="Semantik der Diff-Sprache" VSHIFT="5">
<node CREATED="1472216261102" ID="ID_1057680407" MODIFIED="1472216264937" TEXT="sinnvoll definiert"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1472216265486" FOLDED="true" ID="ID_1774905589" MODIFIED="1488423308101" TEXT="sp&#xe4;ter re-evaluieren (Ticket #996)">
<node CREATED="1472216306208" FOLDED="true" ID="ID_310892577" MODIFIED="1488423308086" TEXT="AFTER">
<arrowlink DESTINATION="ID_1957591180" ENDARROW="Default" ENDINCLINATION="-27;14;" ID="Arrow_ID_228215960" STARTARROW="None" STARTINCLINATION="139;3;"/>
<node CREATED="1473040688153" ID="ID_979711393" MODIFIED="1473040722530" TEXT="bleibt vorerst erhalten">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1473040705207" ID="ID_14067702" MODIFIED="1473040710378" TEXT="kann sinnvoll implementiert werden"/>
<node CREATED="1473040711582" ID="ID_1480176374" MODIFIED="1473040717859" TEXT="ist sehr n&#xfc;tzlich">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1472219973288" FOLDED="true" ID="ID_587190582" MODIFIED="1488423308087" TEXT="Ref::THIS">
<icon BUILTIN="button_cancel"/>
<node CREATED="1472220009699" ID="ID_1534445515" MODIFIED="1472220054293">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>wenn &#252;berhaupt, </i>dann im Matcher im Binding-Layer implementieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
<node CREATED="1473040485789" ID="ID_1621538037" MODIFIED="1473040633258" TEXT="nein: f&#xfc;hrt zu verworrener Implementierung bzw. ist unm&#xf6;glich">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn wir haben nun mehrere Layer,
    </p>
    <p>
      und der Selector kann einfach anhand von Ref::THIS keine sinnvolle Entscheidung treffen.
    </p>
    <p>
      Daher versuchen dann alle Layer dieses Element zu behandeln, oder gar keiner
    </p>
    <p>
      
    </p>
    <p>
      Und da der Selector <i>nur die Spec anschauen darf, </i>l&#228;&#223;t sich das auch nachher nicht mehr korrigieren
    </p>
    <p>
      
    </p>
    <p>
      Daher habe ich mich <b>entschlossen, dieses Sprachkonstrukt zu entfernen</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1473040662653" ID="ID_397737711" MODIFIED="1473040680738">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>entfernt</b>, da schlechtes Design
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1472216312295" FOLDED="true" ID="ID_149667448" MODIFIED="1488423308087" TEXT="Ref::CHILD">
<icon BUILTIN="button_cancel"/>
<node CREATED="1472219981838" ID="ID_117724515" MODIFIED="1472219987506" TEXT="was genau soll das sein??"/>
<node CREATED="1472219987983" ID="ID_555190066" MODIFIED="1472219997032" TEXT="was ist der Unterschied zu Ref::THIS"/>
<node CREATED="1472219998405" ID="ID_1294425222" MODIFIED="1472220005255" TEXT="funktioniert das &#xfc;berhaupt noch??"/>
<node CREATED="1473040662653" ID="ID_1838404392" MODIFIED="1473040680738">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>entfernt</b>, da schlechtes Design
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1458850387823" FOLDED="true" HGAP="19" ID="ID_415691337" MODIFIED="1488423308115" TEXT="allgemein" VSHIFT="13">
<node CREATED="1458850397158" FOLDED="true" ID="ID_1336568549" MODIFIED="1488423308101" TEXT="GenNode">
<node CREATED="1458850406396" FOLDED="true" ID="ID_1837583550" MODIFIED="1488423308087" TEXT="string-Repr&#xe4;sentation der Payload">
<icon BUILTIN="hourglass"/>
<node CREATED="1473352291785" ID="ID_1639951853" MODIFIED="1473352300715" TEXT="vorerst nicht ben&#xf6;tigt"/>
<node CREATED="1473352306190" ID="ID_1647542723" MODIFIED="1473352310810" TEXT="w&#xe4;re besser da&#xdf; nicht"/>
<node CREATED="1473352312206" ID="ID_1150422227" MODIFIED="1473352316377" TEXT="Entscheidung vertagt"/>
</node>
<node COLOR="#338800" CREATED="1458850422587" ID="ID_1552868803" MODIFIED="1458851592866" TEXT="Metafunktion f&#xfc;r m&#xf6;gliche Payload">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1458850437017" FOLDED="true" ID="ID_592865755" MODIFIED="1488423308101" TEXT="Planting-Handle">
<node CREATED="1458850456134" ID="ID_1020391699" MODIFIED="1473352276059" TEXT="Zwischenschicht einziehen im TreeMutator">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      anscheinend nicht notwendig
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1458850463389" ID="ID_769403943" MODIFIED="1473352257250" TEXT="Generische Impl herausl&#xf6;sen">
<icon BUILTIN="button_ok"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1458850471228" ID="ID_167381938" MODIFIED="1473352333631" TEXT="Unit-Test hierf&#xfc;r">
<icon BUILTIN="bell"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1473352396906" FOLDED="true" HGAP="-48" ID="ID_392196966" MODIFIED="1491009383729" TEXT="Integration" VSHIFT="25">
<icon BUILTIN="pencil"/>
<node CREATED="1473352465473" FOLDED="true" ID="ID_158999012" MODIFIED="1488423342547" TEXT="in Tangible">
<icon BUILTIN="button_ok"/>
<node CREATED="1475250911087" FOLDED="true" ID="ID_1042895809" MODIFIED="1488423308115" TEXT="zu kl&#xe4;ren">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1475250919286" FOLDED="true" ID="ID_211228413" MODIFIED="1488423308101" TEXT="wie sieht die initiale (diff)Nachricht aus">
<icon BUILTIN="help"/>
<node CREATED="1475355977081" ID="ID_1302138609" MODIFIED="1475355984716" TEXT="dediziertes API"/>
<node CREATED="1475356001462" ID="ID_1339165799" MODIFIED="1475356007240" TEXT="Nexus zerlegt das"/>
<node CREATED="1475356007860" ID="ID_1405013613" MODIFIED="1475356015255" TEXT="l&#xf6;st direkt Anwendung aus"/>
</node>
<node CREATED="1475251507800" FOLDED="true" ID="ID_1645310973" MODIFIED="1488423308102" TEXT="wo/wie wird Diff-Nachricht behandelt">
<icon BUILTIN="help"/>
<node CREATED="1475355925880" ID="ID_464242009" MODIFIED="1487275490746" TEXT="...analog zur &quot;mark&quot;-Nachricht">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <ul>
      <li>
        diese wird im Nexus behandelt, in dem die Tangible::mark()-Methode aktiviert wird
      </li>
      <li>
        in dieser wiederum steckt eine Default-Handler-Sequenz, plus ein Strategy-Pattern
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1475250930756" FOLDED="true" ID="ID_509435943" MODIFIED="1488423308102" TEXT="wo steckt der Diff selber">
<icon BUILTIN="help"/>
<node CREATED="1475251013513" ID="ID_1464747905" MODIFIED="1475251058167" TEXT="Diff == iterierbare &lt;DiffStep&gt;-Sequenz">
<icon BUILTIN="info"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1475251032255" ID="ID_824967618" MODIFIED="1475251053067" TEXT="pa&#xdf;t nicht in eine GenNode">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1475355960899" ID="ID_1468155641" MODIFIED="1475355974481">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Marker-Typ <b>MutationMessage</b>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1475250947858" FOLDED="true" ID="ID_1438298132" MODIFIED="1488423308102" TEXT="wer ist der Kommunikationspartner">
<icon BUILTIN="help"/>
<node CREATED="1475356021762" ID="ID_1261042951" MODIFIED="1475356028310" TEXT="irrelevant, da abstrahiert"/>
</node>
<node CREATED="1475250957097" ID="ID_1839306136" MODIFIED="1475250969909" TEXT="wie kennt das Tangible diesen Partner">
<icon BUILTIN="help"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1475342135193" FOLDED="true" HGAP="31" ID="ID_336806935" MODIFIED="1488423308115" TEXT="Problem: Diff-Nachricht" VSHIFT="7">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1475342157302" ID="ID_1383404259" MODIFIED="1475342166025" TEXT="&quot;Diff&quot; ist kein Typ"/>
<node CREATED="1475342185707" ID="ID_787925049" MODIFIED="1475342190662" TEXT="Lumiera-Iteratoren sind generisch"/>
<node CREATED="1475342218598" FOLDED="true" ID="ID_686706998" MODIFIED="1488423308102" TEXT="Alternativen">
<icon BUILTIN="idea"/>
<node CREATED="1475342242123" FOLDED="true" ID="ID_996905586" MODIFIED="1488423308087" TEXT="implizite Diff-Facade">
<icon BUILTIN="button_cancel"/>
<node CREATED="1475342267168" ID="ID_1015562836" MODIFIED="1475342372677" TEXT="nur ID eines Diff senden..."/>
<node CREATED="1475342283158" ID="ID_859890341" MODIFIED="1475342287497" TEXT="Diff ist dann hinterlegt"/>
<node CREATED="1475342309914" ID="ID_1398047498" MODIFIED="1475342360640" TEXT="Wo? im GuiModel??">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1475342301331" ID="ID_496549205" MODIFIED="1475342346474" TEXT="zwar implizit, aber starr">
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node CREATED="1475342230013" FOLDED="true" ID="ID_129545687" MODIFIED="1488423308087" TEXT="UI-Bus-Interface erweitern">
<icon BUILTIN="button_ok"/>
<node CREATED="1475342469085" ID="ID_218240539" MODIFIED="1475342487060" TEXT="Ausgangspunkt: DiffMutable">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1475342391631" ID="ID_1330153391" MODIFIED="1475342446252" TEXT="Platzhalter-Typ: DiffMessage"/>
<node CREATED="1475342401278" ID="ID_1020722080" MODIFIED="1475342430750" TEXT="macht Diff zug&#xe4;nglich"/>
</node>
</node>
<node CREATED="1475444207483" ID="ID_920212553" MODIFIED="1475444236127" TEXT="Festlegung: Tangible als Zieltyp">
<linktarget COLOR="#9287b2" DESTINATION="ID_920212553" ENDARROW="Default" ENDINCLINATION="398;0;" ID="Arrow_ID_52222580" SOURCE="ID_62520790" STARTARROW="None" STARTINCLINATION="398;0;"/>
<icon BUILTIN="yes"/>
</node>
<node COLOR="#338800" CREATED="1475356047496" FOLDED="true" HGAP="29" ID="ID_667427572" MODIFIED="1488423308102" TEXT="Implementierung" VSHIFT="14">
<icon BUILTIN="button_ok"/>
<node CREATED="1475356059342" ID="ID_1039482059" MODIFIED="1475444176904" TEXT="wie wird Diff eingebettet">
<icon BUILTIN="help"/>
</node>
<node CREATED="1475439403514" FOLDED="true" ID="ID_1619035258" MODIFIED="1488423308087" TEXT="Entscheidung">
<icon BUILTIN="yes"/>
<node CREATED="1475439416185" ID="ID_963854956" MODIFIED="1475444176904" TEXT="Builder-Funktion nicht sinnvoll">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....denn dann m&#252;&#223;te der Benutzer die Mechanik sehr genau verstehen, und stets eine auto-Variable definieren.
    </p>
    <p>
      Sinnvoll w&#228;re dieser Ansatz nur, wenn das UI-Bus-API eine MutationMessage const&amp; nehmen w&#252;rde,
    </p>
    <p>
      denn dann k&#246;nnte man den Builder-Aufruf inline schreiben.
    </p>
    <p>
      Da wir aber stets den Diff moven und dann iterieren, scheidet const&amp; aus
    </p>
    <p>
      Und f&#252;r eine reine Ref erzeugt C++ niemals eine anonyme Instanz!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1475439429055" ID="ID_1383518557" MODIFIED="1475444176904" TEXT="also: embeded Buffer">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und diesen mit VTable best&#252;cken.
    </p>
    <p>
      Daf&#252;r wird die &#228;u&#223;ere H&#252;lle non-virtual
    </p>
    <p>
      und kann noncopyable gemacht werden..
    </p>
    <p>
      Das erlaubt dem Benutzer, einfach zu schreiben
    </p>
    <p>
      MutationMessage(blaBlubb())
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1475439436134" ID="ID_583247605" MODIFIED="1475444176904" TEXT="verwende InPlaceBuffer"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1475356066853" FOLDED="true" ID="ID_821643910" MODIFIED="1488423308087" TEXT="Problem: Puffergr&#xf6;&#xdf;e">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1475444135180" ID="ID_1434100612" MODIFIED="1475444176904" TEXT="sinnvoll: Spezialfall f&#xfc;r Tangible"/>
<node CREATED="1475444145458" ID="ID_62520790" MODIFIED="1475444244452" TEXT="kann dann DiffSizeTraits spezialisieren">
<arrowlink COLOR="#9287b2" DESTINATION="ID_920212553" ENDARROW="Default" ENDINCLINATION="398;0;" ID="Arrow_ID_52222580" STARTARROW="None" STARTINCLINATION="398;0;"/>
</node>
</node>
</node>
</node>
<node CREATED="1475449460694" FOLDED="true" HGAP="13" ID="ID_425831735" MODIFIED="1488423308115" TEXT="Prototyp: MockElm" VSHIFT="32">
<icon BUILTIN="button_ok"/>
<node CREATED="1475449491378" ID="ID_1309107114" MODIFIED="1475449505548" TEXT="f&#xfc;hre Spiel-Attribute und Kinder ein"/>
<node CREATED="1475449507224" FOLDED="true" ID="ID_1783640184" MODIFIED="1488423308102" TEXT="Kinder">
<node CREATED="1475449530565" ID="ID_284330532" MODIFIED="1475449531825" TEXT="collection von MockElm"/>
<node CREATED="1475449532556" ID="ID_1804332542" MODIFIED="1487275490779" TEXT="MockElm ist noncopyable">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...aus gutem Grund!
    </p>
    <p>
      Der Nexus speichert n&#228;mlich eine direkte Referenz in der Routingtabelle
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1475449544379" ID="ID_1132064603" MODIFIED="1475449583284" TEXT="also nur per shared_ptr">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1475449589725" FOLDED="true" ID="ID_460570633" MODIFIED="1488423308102" TEXT="Attribute">
<node CREATED="1475449594060" FOLDED="true" HGAP="27" ID="ID_1311933287" MODIFIED="1488423308087" TEXT="Typ" VSHIFT="14">
<icon BUILTIN="help"/>
<node CREATED="1475449959916" ID="ID_573697431" MODIFIED="1475449966244" TEXT="map&lt;string,string&gt;"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1475449611754" FOLDED="true" HGAP="33" ID="ID_1285123321" MODIFIED="1488423308066" TEXT="Grundsatzfrage: Attribut-Map" VSHIFT="1">
<icon BUILTIN="help"/>
<node CREATED="1475449654357" ID="ID_1802368116" MODIFIED="1475449659480" TEXT="Ordnung / Unordnung"/>
<node CREATED="1475449660020" ID="ID_286191859" MODIFIED="1475449740005" TEXT="Diff geht von Listen-Ordnung aus">
<icon BUILTIN="info"/>
</node>
<node CREATED="1475449777532" ID="ID_476663339" MODIFIED="1475449893670" TEXT="Symptom: Compile-Fehler">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <ul>
      <li>
        Map hat kein emplace_back
      </li>
      <li>
        Map hat kein back()
      </li>
    </ul>
    <p>
      Beides ist erst mal sinnvoll. Map hat zwar ein emplace, aber das f&#252;gt eben <i>irgendwo</i>&#160;ein
    </p>
    <p>
      Und es gibt nicht sowas wie das &quot;zuletzt behandelte&quot; Element
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1475506967550" HGAP="42" ID="ID_1112268061" MODIFIED="1475507021278" TEXT="ist aber implementierbar" VSHIFT="8">
<arrowlink COLOR="#9f96d9" DESTINATION="ID_1853473792" ENDARROW="Default" ENDINCLINATION="41;-121;" ID="Arrow_ID_503281678" STARTARROW="Default" STARTINCLINATION="1873;0;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1475449719180" FOLDED="true" ID="ID_6178930" MODIFIED="1488423307184" TEXT="Sinnvoll das zu erweitern">
<icon BUILTIN="help"/>
<node CREATED="1475450034810" FOLDED="true" ID="ID_445949466" MODIFIED="1488423307184" TEXT="pro">
<icon BUILTIN="button_cancel"/>
<node CREATED="1475450042009" FOLDED="true" ID="ID_1941838908" MODIFIED="1488423307183" TEXT="man k&#xf6;nnte">
<node CREATED="1475450050824" ID="ID_1660008285" MODIFIED="1475450053243" TEXT="ins"/>
<node CREATED="1475450055775" ID="ID_1213643084" MODIFIED="1475450058698" TEXT="after::END"/>
<node CREATED="1475450064470" ID="ID_754885592" MODIFIED="1475450069265" TEXT="assign / mutate"/>
</node>
<node CREATED="1475450148771" FOLDED="true" ID="ID_736367412" MODIFIED="1488423307183">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Reihenfolge
    </p>
    <p>
      erhalten!
    </p>
  </body>
</html></richcontent>
<node CREATED="1475450179591" ID="ID_793985703" MODIFIED="1475450197936" TEXT="dann sogar del"/>
<node CREATED="1475450053927" ID="ID_1110207658" MODIFIED="1475450055018" TEXT="pick"/>
</node>
</node>
<node CREATED="1475450038953" FOLDED="true" ID="ID_1285250844" MODIFIED="1488423307184" TEXT="con">
<icon BUILTIN="button_ok"/>
<node CREATED="1475450285336" ID="ID_288261682" MODIFIED="1475450300712" TEXT="kein Schutz gegen sinnlose Operationen"/>
<node CREATED="1475450229944" ID="ID_957217069" MODIFIED="1475450239042" TEXT="Reihenfolge ist Impl-Detail"/>
<node CREATED="1475450240318" ID="ID_1993698586" MODIFIED="1475450376320" TEXT="Hashmap">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...hat eine &quot;zuf&#228;llige&quot; Reihenfolge, die von den Hash-Werten der gespeicherten Daten abh&#228;ngt.
    </p>
    <p>
      Das bricht mit unserem grunds&#228;tzlichen Konzept der <b>kongruenten</b>&#160; Daten-Strukturen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1475450272442" ID="ID_567969130" MODIFIED="1475450429317" TEXT="inkompatibel mit GenNode">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ein Diff, das von einer ETD gezogen wurde,
    </p>
    <p>
      l&#228;&#223;t sich nicht auf eine Map-Implementierung aufspielen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
<node CREATED="1475506346525" FOLDED="true" ID="ID_574367406" MODIFIED="1488423307184" VSHIFT="6">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
<node CREATED="1475506358863" ID="ID_120379432" MODIFIED="1475506391795" TEXT="wird nicht empfohlen">
<font ITALIC="true" NAME="SansSerif" SIZE="13"/>
</node>
<node CREATED="1475506368078" ID="ID_1568414977" MODIFIED="1475506381941" TEXT="vollst&#xe4;ndigkeitshalber unterst&#xfc;tzt">
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1475506411768" ID="ID_583086851" MODIFIED="1475506507414" TEXT="kann sinnvoll sein...">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...zum Beispiel wie grade hier, beim MockElm
    </p>
    <p>
      das wird vermutlich niemals wirklich in einem vollen Diff-Zusammenhang gebraucht.
    </p>
    <p>
      
    </p>
    <p>
      Und dann ist unbestreitbar eine Map eine sehr einfache Implementierung
    </p>
    <p>
      und auch im Diff-Applikator nicht wirklich schwierig zu unterst&#252;tzen
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="10"/>
</node>
<node COLOR="#338800" CREATED="1475506514203" FOLDED="true" HGAP="45" ID="ID_1183193114" MODIFIED="1488423307184" TEXT="Begr&#xfc;ndung" VSHIFT="23">
<node CREATED="1475506528937" ID="ID_1971084550" MODIFIED="1475506538324" TEXT="es verletzt die Prinzipien"/>
<node CREATED="1475506538983" ID="ID_1555995664" MODIFIED="1475506547491" TEXT="insofern aber wie Feld-Attribute auch"/>
<node CREATED="1475506549886" ID="ID_275560362" MODIFIED="1475506558609" TEXT="eingeschr&#xe4;nkt unterst&#xfc;tzbar"/>
<node CREATED="1475506559309" ID="ID_1767038604" MODIFIED="1475506896798" TEXT="Objekt -&gt; ETD -&gt; Objekt">
<arrowlink COLOR="#807bce" DESTINATION="ID_1808149320" ENDARROW="Default" ENDINCLINATION="671;0;" ID="Arrow_ID_1677374853" STARTARROW="Default" STARTINCLINATION="994;0;"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1475546298721" HGAP="24" ID="ID_1646756860" MODIFIED="1475546316260" TEXT="vorkonfiguriertes TreeMutator-Binding" VSHIFT="10">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1473352470896" FOLDED="true" HGAP="21" ID="ID_864450713" MODIFIED="1491009381377" TEXT="Unit-Tests" VSHIFT="18">
<node COLOR="#338800" CREATED="1473352475375" ID="ID_731425414" MODIFIED="1475546284267" TEXT="AbstractTangible_test">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1473352482502" ID="ID_1891838260" MODIFIED="1473352485210" TEXT="UiBus_test"/>
</node>
</node>
</node>
<node CREATED="1448658726090" HGAP="18" ID="ID_37610818" MODIFIED="1488423307195" TEXT="Commands" VSHIFT="36">
<node CREATED="1448658755071" FOLDED="true" ID="ID_1033500384" MODIFIED="1488423342666" TEXT="wie definieren">
<icon BUILTIN="help"/>
<node CREATED="1448658974985" ID="ID_974207484" MODIFIED="1448658989011" TEXT="Definition braucht Session-Modell"/>
<node CREATED="1448659018412" FOLDED="true" ID="ID_1988707066" MODIFIED="1488423342548" TEXT="zentral">
<icon BUILTIN="button_ok"/>
<node CREATED="1448659021803" ID="ID_1800666256" MODIFIED="1448659025502" TEXT="nah an der Session"/>
<node CREATED="1448659026042" ID="ID_831434285" MODIFIED="1448659032566" TEXT="ein Schnittstellen-Paket"/>
<node CREATED="1448669306480" ID="ID_1009169288" MODIFIED="1483910782488">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem: <b><font color="#8d02e1">InteractionControl</font></b>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#5f0ac8" DESTINATION="ID_329609486" ENDARROW="Default" ENDINCLINATION="219;-148;" ID="Arrow_ID_1101633958" STARTARROW="None" STARTINCLINATION="-315;0;"/>
<linktarget COLOR="#b567c4" DESTINATION="ID_1009169288" ENDARROW="Default" ENDINCLINATION="254;0;" ID="Arrow_ID_200182911" SOURCE="ID_1002329025" STARTARROW="None" STARTINCLINATION="162;0;"/>
</node>
</node>
<node CREATED="1448659051823" FOLDED="true" ID="ID_1995941804" MODIFIED="1488423342548" TEXT="dezentral">
<icon BUILTIN="button_cancel"/>
<node CREATED="1448659059734" ID="ID_1682547207" MODIFIED="1448659065617" TEXT="im GUI-Kontext"/>
<node CREATED="1448659079044" ID="ID_508900352" MODIFIED="1448659121799" TEXT="mu&#xdf; Session referenzieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...was andernfalles komplett vermeidbar w&#228;re,
    </p>
    <p>
      da im &#220;brigen das UI-Modell nur mit LUIDs und generischen Namen arbeitet
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1448658773972" FOLDED="true" ID="ID_1826752340" MODIFIED="1488423342666" TEXT="referenzieren">
<node CREATED="1448659283641" ID="ID_209203268" MODIFIED="1448659290276" TEXT="letztlich ein String"/>
<node CREATED="1448659347728" ID="ID_508578010" MODIFIED="1448659355835" TEXT="Konstante bei der Command-Funktion"/>
<node CREATED="1448659334058" ID="ID_683316035" MODIFIED="1448659340357" TEXT="Makro + Namenskonvention"/>
<node CREATED="1448683561882" ID="ID_839961131" MODIFIED="1448691299107">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      besser: <b>InvocationTrail</b>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#81759f" DESTINATION="ID_1679641405" ENDARROW="Default" ENDINCLINATION="604;-108;" ID="Arrow_ID_1210299906" STARTARROW="None" STARTINCLINATION="702;0;"/>
<arrowlink COLOR="#aaa9c1" DESTINATION="ID_1193936433" ENDARROW="Default" ENDINCLINATION="392;0;" ID="Arrow_ID_1338946264" STARTARROW="Default" STARTINCLINATION="392;0;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1448658819990" FOLDED="true" ID="ID_405705528" MODIFIED="1488423342666" TEXT="API explizit">
<icon BUILTIN="help"/>
<node CREATED="1448658861760" FOLDED="true" ID="ID_1373851015" MODIFIED="1488423342548" TEXT="pro">
<node CREATED="1448658867680" ID="ID_766147415" MODIFIED="1448658870931" TEXT="sauber"/>
<node CREATED="1448659206251" ID="ID_1542057563" MODIFIED="1448659212278" TEXT="nah am konkreten Widget"/>
<node CREATED="1448659217489" ID="ID_214385592" MODIFIED="1448659227396" TEXT="integriert in sonstiges UI-Verhalten"/>
<node CREATED="1448658947429" ID="ID_302940509" MODIFIED="1448659157613" TEXT="Fehlaufrufe ausgeschlossen"/>
</node>
<node CREATED="1448658873767" FOLDED="true" ID="ID_1028163948" MODIFIED="1488423342548" TEXT="con">
<node CREATED="1448658883341" ID="ID_543652783" MODIFIED="1448658885457" TEXT="Aufwand"/>
<node CREATED="1448658885973" ID="ID_1734626095" MODIFIED="1448658940946" TEXT="ctxt-Men&#xfc; aufbauen"/>
<node CREATED="1448659158177" ID="ID_1496901578" MODIFIED="1448659162165" TEXT="Namen m&#xfc;ssen matchen"/>
<node CREATED="1448659163089" ID="ID_431765107" MODIFIED="1448659252553" TEXT="man mu&#xdf; Korrektheit pr&#xfc;fen"/>
<node CREATED="1448683674443" ID="ID_1931753413" MODIFIED="1448683684206" TEXT="Einheitlichkeit gef&#xe4;hrdet"/>
</node>
</node>
<node CREATED="1448683454504" FOLDED="true" HGAP="42" ID="ID_510876193" MODIFIED="1488423342666" TEXT="API generisch" VSHIFT="-1">
<icon BUILTIN="button_ok"/>
<node CREATED="1448683636672" ID="ID_1485949406" MODIFIED="1448683647815" TEXT="verhindert Wildwuchs"/>
<node CREATED="1448683475853" ID="ID_24599710" MODIFIED="1448683492586">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Command und Verhaltensmuster
    </p>
    <p>
      bleiben zusammen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448683496794" FOLDED="true" ID="ID_821199097" MODIFIED="1488423342548" TEXT="unterst&#xfc;tzt weiterreichende Konzepte">
<node CREATED="1448683518496" ID="ID_1987782234" MODIFIED="1448683521843" TEXT="Perspektiven"/>
<node CREATED="1448683522359" ID="ID_723377610" MODIFIED="1448683525346" TEXT="Fokus-Konzept"/>
<node CREATED="1448683525822" ID="ID_1395068730" MODIFIED="1448683529258" TEXT="Gesten"/>
</node>
</node>
<node CREATED="1448691191042" FOLDED="true" HGAP="35" ID="ID_1710578352" MODIFIED="1488423342666" TEXT="Lebenszyklus" VSHIFT="-1">
<font NAME="SansSerif" SIZE="13"/>
<node CREATED="1448691210544" ID="ID_242337741" MODIFIED="1448691218906" TEXT="Command-Skript: im Code"/>
<node CREATED="1448691219614" FOLDED="true" ID="ID_992447056" MODIFIED="1488423342548" TEXT="Bildungs-Regeln: ebenda">
<node CREATED="1448691243259" ID="ID_692910736" MODIFIED="1448691256433" TEXT="Frage: wie injizieren">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1448691264512" FOLDED="true" ID="ID_1193936433" MODIFIED="1488423342548" TEXT="InvocationTrail erstellen">
<linktarget COLOR="#aaa9c1" DESTINATION="ID_1193936433" ENDARROW="Default" ENDINCLINATION="392;0;" ID="Arrow_ID_1338946264" SOURCE="ID_839961131" STARTARROW="Default" STARTINCLINATION="392;0;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1450490467952" ID="ID_1453525628" MODIFIED="1450490480705" TEXT="verwendet zwei GenNode-bindings"/>
<node CREATED="1450490481270" ID="ID_1603770702" MODIFIED="1450490504407" TEXT="fertig implementiert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...was ich einen Monat sp&#228;ter schon wieder vergessen hatte...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1448691315849" FOLDED="true" ID="ID_1198560195" MODIFIED="1488423342548" TEXT="Bindugs-Regel">
<icon BUILTIN="hourglass"/>
<node CREATED="1448691343406" ID="ID_642235825" MODIFIED="1448691350558" TEXT="funktor"/>
<node CREATED="1448691324304" ID="ID_1103199702" MODIFIED="1448691338522" TEXT="definiert Bezugs-Quellen"/>
<node CREATED="1448691355340" ID="ID_724965495" MODIFIED="1448691465633" TEXT="liefert Record&lt;GenNode&gt;"/>
<node CREATED="1452560897720" ID="ID_1869468443" MODIFIED="1452561022338" TEXT="vorerst noch nicht impl...">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hier geht es darum, eine Regel zu generieren,
    </p>
    <p>
      die dann den zugrundeliegenden Command-Prototyp automatisch mit konkreten Aufrufparametern binden kann,
    </p>
    <p>
      sobald bestimmte Umst&#228;nde im UI einschl&#228;gig werden
    </p>
    <p>
      
    </p>
    <p>
      Das kann ich f&#252;r die ersten Tests auslassen, und stattdessen einfach
    </p>
    <p>
      den InvocationTrail manuell im Testcode binden
    </p>
  </body>
</html></richcontent>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1448691865257" FOLDED="true" ID="ID_1343859473" MODIFIED="1488423342548" TEXT="Command-Binding wird gesendet">
<node CREATED="1448691954533" ID="ID_966334383" MODIFIED="1448691960295" TEXT="Performance">
<icon BUILTIN="help"/>
</node>
<node CREATED="1448691962068" ID="ID_1045203424" MODIFIED="1448691974575" TEXT="nicht &quot;auf Verdacht&quot; senden">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1448692020868" ID="ID_705436290" MODIFIED="1448692192462" TEXT="Begr&#xfc;ndung: kein Kopieren der Argumente">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die Alternative w&#228;re, den Record mit allen Argumenten in den InvocationTrail zu packen.
    </p>
    <p>
      Dann w&#252;rden wir diesen aber weiter versenden, um Aktivierungen zuzustellen.
    </p>
    <p>
      Das w&#252;rde bedeuten, die Argumente x-fach zu kopieren (oder mich zu einem ref-counting-Mechanismus zwingen)
    </p>
    <p>
      
    </p>
    <p>
      Daher ist es besser, <i>einmal</i>, wenn die Argumente bekannt werden, diese zum Prototypen zu schicken
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1450490565739" ID="ID_849102091" MODIFIED="1452557610273" TEXT="implementiert ist: Erstellen im InvocationTrail">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1452561062226" ID="ID_444925471" MODIFIED="1454978022494" TEXT="convenience overload f&#xfc;r mehrere Argumente">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint, das Model-Element (Tangible) sollte einen solchen Overload anbieten,
    </p>
    <p>
      der unimttelbar Datenwerte nimmt und sie in einen Argument-Record packt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1450490580473" ID="ID_925248058" MODIFIED="1454975395091" TEXT="Empfangen und Binden">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1448691875184" ID="ID_642935499" MODIFIED="1448691904352" TEXT="f&#xfc;r die Invocation gen&#xfc;gt dann die reine ID"/>
<node CREATED="1448692196213" FOLDED="true" ID="ID_727542699" MODIFIED="1488423342548">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Wichtig <font color="#e10409">(offen)</font>: Instanz-Management
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1448692240015" ID="ID_58254695" MODIFIED="1448692245826" TEXT="es geht um Allokationen in Proc"/>
<node CREATED="1448692247478" ID="ID_1199512787" MODIFIED="1448692258689" TEXT="jede Instanz == Eintrag in der Command-Registry"/>
<node CREATED="1448692287329" ID="ID_1099768193" MODIFIED="1448692317249">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Regel: nur was sich parallel entwickeln kann,
    </p>
    <p>
      mu&#223; auch geforkt werden
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1483925128784" FOLDED="true" HGAP="28" ID="ID_876058329" MODIFIED="1488423308115" TEXT="UI-Bus Protokoll-Erweiterung" VSHIFT="23">
<arrowlink COLOR="#e0ac6d" DESTINATION="ID_242219866" ENDARROW="Default" ENDINCLINATION="-2132;93;" ID="Arrow_ID_1940592535" STARTARROW="Default" STARTINCLINATION="2588;-604;"/>
<icon BUILTIN="help"/>
<node CREATED="1483925141934" ID="ID_981015562" MODIFIED="1483925151561" TEXT="Nachricht um zu forken"/>
<node CREATED="1483925152477" ID="ID_467666678" MODIFIED="1483925163288" TEXT="erweiterte Command-ID als Antwort"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1448669332204" HGAP="76" ID="ID_329609486" MODIFIED="1483910973175" TEXT="InteractionControl">
<arrowlink COLOR="#b694bf" DESTINATION="ID_430949603" ENDARROW="Default" ENDINCLINATION="178;425;" ID="Arrow_ID_1062305149" STARTARROW="Default" STARTINCLINATION="-629;-2354;"/>
<linktarget COLOR="#5f0ac8" DESTINATION="ID_329609486" ENDARROW="Default" ENDINCLINATION="219;-148;" ID="Arrow_ID_1101633958" SOURCE="ID_1009169288" STARTARROW="None" STARTINCLINATION="-315;0;"/>
<font NAME="SansSerif" SIZE="15"/>
<node CREATED="1448669439582" HGAP="41" ID="ID_307071507" MODIFIED="1488423307196" TEXT="ad-hoc oder systematisch" VSHIFT="-2">
<icon BUILTIN="help"/>
<node CREATED="1448669512988" ID="ID_1184892495" MODIFIED="1448669517328" TEXT="fundamentale Frage"/>
<node CREATED="1448669517796" ID="ID_1165902172" MODIFIED="1448669526326" TEXT="f&#xfc;r mich eigentlich sofort klar"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1448669527274" ID="ID_129855368" MODIFIED="1448669537302" TEXT="aber Vorsicht">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1448669556607" ID="ID_493021007" MODIFIED="1448683719946" TEXT="das wird nie fertig...">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1448669562478" ID="ID_869338862" MODIFIED="1448683732978" TEXT="niemand wird es verstehen">
<icon BUILTIN="smiley-angry"/>
</node>
<node CREATED="1448669567765" ID="ID_1254931544" MODIFIED="1448669587150" TEXT="b&#xfc;rstet die UI-Toolkits &#xbb;gegen den Strich&#xab;"/>
</node>
</node>
<node CREATED="1448669636900" HGAP="51" ID="ID_1111001545" MODIFIED="1487270090214" TEXT="Einf&#xfc;hren einer Zwischenebene" VSHIFT="-24">
<node CREATED="1448669646474" ID="ID_1590745326" MODIFIED="1448669657189" TEXT="hyper-r&#xe4;umliche Anordnung"/>
<node CREATED="1448669657633" ID="ID_1054403772" MODIFIED="1448669661661" TEXT="Focus-Bewegung"/>
<node CREATED="1448669662417" ID="ID_828883183" MODIFIED="1448669674907" TEXT="Einteilung in Arbeits-R&#xe4;ume"/>
<node CREATED="1448669679694" ID="ID_729190965" MODIFIED="1448669698124" TEXT="Konfigurierbarkeit"/>
<node CREATED="1448669675495" ID="ID_1257367197" MODIFIED="1448669679146" TEXT="Perspektiven"/>
<node CREATED="1448669699148" ID="ID_1264820517" MODIFIED="1448669700591" TEXT="Gesten"/>
</node>
<node CREATED="1448669742334" HGAP="28" ID="ID_1679641405" MODIFIED="1487271385158" TEXT="InteractionStateManager" VSHIFT="-3">
<linktarget COLOR="#81759f" DESTINATION="ID_1679641405" ENDARROW="Default" ENDINCLINATION="604;-108;" ID="Arrow_ID_1210299906" SOURCE="ID_839961131" STARTARROW="None" STARTINCLINATION="702;0;"/>
<font BOLD="true" NAME="SansSerif" SIZE="13"/>
<node CREATED="1448669816476" ID="ID_1895677880" MODIFIED="1448669822183" TEXT="lauscht als Observer mit"/>
<node CREATED="1448669824243" ID="ID_1225519733" MODIFIED="1448669832398" TEXT="h&#xe4;ngt sich in diverse Punkte ein"/>
<node CREATED="1448669838745" ID="ID_1663939304" MODIFIED="1448669848204" TEXT="h&#xe4;llt ein lokales Zustansmodell"/>
<node CREATED="1448669854119" ID="ID_874080079" MODIFIED="1448669857794" TEXT="wertet Regeln aus"/>
<node CREATED="1448669869973" ID="ID_1259588429" MODIFIED="1448669888646" TEXT="sendet Enablement">
<node CREATED="1448669889514" ID="ID_1191247113" MODIFIED="1448669898645" TEXT="an die konkreten Trigger-Orte"/>
<node CREATED="1448669899113" ID="ID_990871123" MODIFIED="1450228230836" TEXT="er sendet jeweils einen CommandInvocationTrail"/>
</node>
</node>
<node CREATED="1487213803525" HGAP="26" ID="ID_735655054" MODIFIED="1487273346373" TEXT="InteractionDirector" VSHIFT="-2">
<arrowlink COLOR="#1b3863" DESTINATION="ID_362749694" ENDARROW="Default" ENDINCLINATION="-518;1012;" ID="Arrow_ID_959577029" STARTARROW="Default" STARTINCLINATION="-376;-568;"/>
<linktarget COLOR="#586d86" DESTINATION="ID_735655054" ENDARROW="Default" ENDINCLINATION="549;166;" ID="Arrow_ID_1031406920" SOURCE="ID_1980246174" STARTARROW="None" STARTINCLINATION="1;-366;"/>
<font NAME="SansSerif" SIZE="14"/>
<node CREATED="1487270206369" ID="ID_582649017" MODIFIED="1487270223050" TEXT="betreibt SpotLocator"/>
<node CREATED="1487275297855" ID="ID_417844993" MODIFIED="1487275301530" TEXT="betreibt FocusTracker"/>
<node CREATED="1487273437019" ID="ID_1600127727" MODIFIED="1487273440390" TEXT="betreibt Navigator"/>
</node>
<node CREATED="1487270226342" ID="ID_1501993794" MODIFIED="1487270229593" TEXT="SpotLocator">
<node CREATED="1487270240084" ID="ID_1171094973" MODIFIED="1487270974510" TEXT="Erf&#xfc;llungsgehilfe">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1487270245884" ID="ID_1688031043" MODIFIED="1487270249375" TEXT="zwei Aspekte">
<node CREATED="1487270262137" FOLDED="true" ID="ID_1256894684" MODIFIED="1488423342666" TEXT="Query">
<node CREATED="1487270288630" ID="ID_744259530" MODIFIED="1487270293609" TEXT="&quot;wo sind wir&quot;?"/>
<node CREATED="1487270294269" ID="ID_903348018" MODIFIED="1487270315902" TEXT="aktuelle WorkSite"/>
<node CREATED="1487270397967" ID="ID_1021036860" MODIFIED="1487270402138" TEXT="LRU history"/>
<node CREATED="1487270732305" ID="ID_702780522" MODIFIED="1487270736429" TEXT="current MediaObject"/>
</node>
<node CREATED="1487270263705" FOLDED="true" ID="ID_302637820" MODIFIED="1488423342666" TEXT="Act">
<node CREATED="1487270473421" ID="ID_911216598" MODIFIED="1487270477624" TEXT="go to FocusPath"/>
</node>
</node>
</node>
<node CREATED="1487270846218" ID="ID_419002057" MODIFIED="1487270848613" TEXT="WorkSite">
<node CREATED="1487270854793" ID="ID_1953389172" MODIFIED="1487270857285" TEXT="management">
<node CREATED="1487270860976" FOLDED="true" ID="ID_1497253556" MODIFIED="1488423342666" TEXT="Speicher">
<node CREATED="1487270865216" ID="ID_685718473" MODIFIED="1487270867635" TEXT="ref counted"/>
<node CREATED="1487270874103" ID="ID_1447761407" MODIFIED="1487270876306" TEXT="registration"/>
</node>
<node CREATED="1487270895587" FOLDED="true" ID="ID_167087353" MODIFIED="1488423342666" TEXT="established">
<node CREATED="1487270906322" ID="ID_1777452351" MODIFIED="1487270908365" TEXT="automatically"/>
<node CREATED="1487270909074" ID="ID_837195046" MODIFIED="1487270911037" TEXT="explicitly"/>
<node CREATED="1487270914353" FOLDED="true" ID="ID_810579522" MODIFIED="1488423342549" TEXT="derived">
<node CREATED="1487270924112" ID="ID_594873450" MODIFIED="1487270926371" TEXT="split"/>
<node CREATED="1487270926887" ID="ID_1400861012" MODIFIED="1487270927931" TEXT="clone"/>
</node>
</node>
</node>
<node CREATED="1487271257011" ID="ID_322579744" MODIFIED="1487271263454" TEXT="Bezugs-Rahmen">
<node CREATED="1487271264969" ID="ID_1714809239" MODIFIED="1487271270093" TEXT="Befehle/Aktionen binden"/>
<node CREATED="1487271279399" ID="ID_1349326625" MODIFIED="1487271281035" TEXT="Toolbar"/>
</node>
<node CREATED="1487272549027" ID="ID_380137967" MODIFIED="1487272552111" TEXT="Abgrenzung">
<node CREATED="1487272553075" ID="ID_743796483" MODIFIED="1487272556990" TEXT="schwierige Aufgabe"/>
<node CREATED="1487272557314" ID="ID_1783972929" MODIFIED="1487272561917" TEXT="mu&#xdf; dynamisch passieren"/>
<node CREATED="1487272580751" ID="ID_1313858813" MODIFIED="1487272585866" TEXT="verlassene WorkSites"/>
<node CREATED="1487273190221" FOLDED="true" ID="ID_1338036086" MODIFIED="1488423342667" TEXT="Genzen">
<node CREATED="1487273192636" ID="ID_1111226069" MODIFIED="1487273196407" TEXT="Dinge innerhalb"/>
<node CREATED="1487273196763" ID="ID_412767726" MODIFIED="1487273199727" TEXT="Dinge au&#xdf;erhalb"/>
<node CREATED="1487273226775" ID="ID_303777362" MODIFIED="1487273231067" TEXT="wann Grenzen anpassen"/>
</node>
<node CREATED="1487273234190" FOLDED="true" ID="ID_752073036" MODIFIED="1488423342667" TEXT="mehrere WorkSites">
<node CREATED="1487273239502" ID="ID_1735700195" MODIFIED="1487273244361" TEXT="k&#xf6;nnen sich &#xfc;berlappen"/>
<node CREATED="1487273245421" ID="ID_1239635545" MODIFIED="1487273249360" TEXT="k&#xf6;nnen naheliegen"/>
</node>
</node>
</node>
<node CREATED="1487210642170" HGAP="28" ID="ID_337241818" MODIFIED="1487273940717" TEXT="Rahmen global" VSHIFT="-1">
<node CREATED="1487210648449" ID="ID_1430752735" MODIFIED="1487273877430" TEXT="konzeptionell">
<arrowlink COLOR="#34dbd3" DESTINATION="ID_1259700796" ENDARROW="Default" ENDINCLINATION="450;232;" ID="Arrow_ID_114106295" STARTARROW="None" STARTINCLINATION="-1346;-1090;"/>
<font ITALIC="true" NAME="SansSerif" SIZE="14"/>
<node CREATED="1487269963098" ID="ID_723170966" MODIFIED="1487269973740" TEXT="Raum als Metapher"/>
<node CREATED="1487269990582" ID="ID_164128588" MODIFIED="1487270005514">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Focus/Spot wird <i>mitbewegt</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1487270008163" ID="ID_1916966167" MODIFIED="1487270010535" TEXT="operational">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1487270012283" ID="ID_212889115" MODIFIED="1487270016268" TEXT="noch unklar">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1487270108238" ID="ID_1957235452" MODIFIED="1487270116960" TEXT="Wechselwirkung: konventioneller Fokus"/>
<node CREATED="1487270131499" ID="ID_1523536395" MODIFIED="1487270139462" TEXT="Observer f&#xfc;r Status-&#xc4;nderungen"/>
<node CREATED="1487270141994" ID="ID_1312242119" MODIFIED="1487270153774" TEXT="Problem: wo andocken?">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1487272791578" ID="ID_1159463727" MODIFIED="1487272793622" TEXT="Strukturen">
<node CREATED="1487272794426" ID="ID_1016756623" MODIFIED="1487272797557" TEXT="transient"/>
<node CREATED="1487272797857" FOLDED="true" ID="ID_1810657142" MODIFIED="1488423342667" TEXT="persistent">
<node CREATED="1487273054439" ID="ID_185665950" MODIFIED="1487273060674" TEXT="Worksites bleiben dauerhaft erhalten"/>
<node CREATED="1487273061054" ID="ID_1347275006" MODIFIED="1487273070912" TEXT="werden im Modell gespeichert"/>
<node CREATED="1487273078564" ID="ID_1502695356" MODIFIED="1487273085526" TEXT="aber an den Benutzer gebunden"/>
</node>
<node CREATED="1487272800417" FOLDED="true" ID="ID_344309590" MODIFIED="1488423342667" TEXT="Wurzel">
<node CREATED="1487273308748" ID="ID_1980246174" MODIFIED="1487273353071" TEXT="Ankerpunkt: InteractionDirector">
<arrowlink COLOR="#586d86" DESTINATION="ID_735655054" ENDARROW="Default" ENDINCLINATION="549;166;" ID="Arrow_ID_1031406920" STARTARROW="None" STARTINCLINATION="1;-366;"/>
</node>
<node CREATED="1487273377603" ID="ID_1842094630" MODIFIED="1487273380871" TEXT="SpotLocator"/>
</node>
<node CREATED="1487272813119" ID="ID_1794274698" MODIFIED="1487272815043" TEXT="Bezug"/>
</node>
</node>
<node CREATED="1487273942038" HGAP="4" ID="ID_1254651309" MODIFIED="1487273950941" TEXT="Dynamik" VSHIFT="19">
<font NAME="SansSerif" SIZE="13"/>
<node CREATED="1487274013493" ID="ID_1949865241" MODIFIED="1487274016432" TEXT="mitbewegen">
<node CREATED="1487274017164" FOLDED="true" ID="ID_1079230717" MODIFIED="1488423342667" TEXT="Trigger">
<node CREATED="1487274021596" ID="ID_1455392363" MODIFIED="1487274026503" TEXT="Fenster wird aktiviert"/>
<node CREATED="1487274135596" ID="ID_1979220741" MODIFIED="1487274142903" TEXT="Keyboard-Focus shift"/>
<node CREATED="1487274145667" ID="ID_771196046" MODIFIED="1487274152518" TEXT="Selection gesetzt"/>
<node CREATED="1487274153058" ID="ID_624086875" MODIFIED="1487274159365" TEXT="Navigation expliizit"/>
</node>
<node CREATED="1487274921058" FOLDED="true" ID="ID_817413556" MODIFIED="1488423342667" TEXT="Ansatzpunkt">
<node CREATED="1487274932472" ID="ID_362677382" MODIFIED="1487274940459" TEXT="wie kann man Trigger installieren?"/>
<node CREATED="1487274942383" FOLDED="true" ID="ID_1727411524" MODIFIED="1488423342549" TEXT="model::Tangible">
<icon BUILTIN="idea"/>
<node CREATED="1487274952662" ID="ID_137454278" MODIFIED="1487274963190" TEXT="immer?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1487274957301" ID="ID_1317780625" MODIFIED="1487274965710" TEXT="explizit?">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1487275149467" ID="ID_46033493" MODIFIED="1487275158357" TEXT="FocusListener"/>
</node>
<node CREATED="1487275167712" FOLDED="true" ID="ID_421642010" MODIFIED="1488423342667" TEXT="FocusTracker">
<node CREATED="1487275245822" ID="ID_1999993873" MODIFIED="1487275254241" TEXT="geh&#xf6;rt zum InteractionDirector"/>
<node CREATED="1487275255092" ID="ID_528280403" MODIFIED="1487275264335" TEXT="mu&#xdf; von jedem Listener erreichbar sein"/>
<node CREATED="1487275265227" ID="ID_1738811487" MODIFIED="1487275276390" TEXT="wie setzt man einen neuen Listener auf?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1487275767543" ID="ID_1953432961" MODIFIED="1487275780067" TEXT="Idee: StateMark -&gt; UI-Bus">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1487275806658" FOLDED="true" ID="ID_46146913" MODIFIED="1488423342549" TEXT="CoreService &lt;-&gt; InteractionDirector">
<node CREATED="1487275821920" ID="ID_1911498338" MODIFIED="1487275828507" TEXT="Verbindung ohnehin notwendig"/>
<node CREATED="1487275829711" ID="ID_467014330" MODIFIED="1487275845616" TEXT="persistenter UI-State"/>
<node CREATED="1487275846133" ID="ID_1450222785" MODIFIED="1487275854287" TEXT="mu&#xdf; im Modell h&#xe4;ngen"/>
<node CREATED="1487275854899" ID="ID_1437855457" MODIFIED="1487275861934" TEXT="und daher unter dem InteractionDirector"/>
</node>
</node>
<node CREATED="1487275415415" FOLDED="true" ID="ID_625776285" MODIFIED="1488423342667" TEXT="Bewegungs-Erkennung">
<node CREATED="1487275419726" FOLDED="true" ID="ID_379201934" MODIFIED="1488423342549" TEXT="ist ein Problem">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1487275427165" ID="ID_196683293" MODIFIED="1487275522436" TEXT="was ist noch in einer WorkSite?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1487275438371" ID="ID_1849848193" MODIFIED="1487275518165" TEXT="wohin bewegen wir uns?">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1487275567114" FOLDED="true" ID="ID_1956672414" MODIFIED="1488423342549" TEXT="Zusammenarbeit">
<node CREATED="1487275572706" FOLDED="true" ID="ID_1943137650" MODIFIED="1488423308115" TEXT="WorkSite">
<node CREATED="1487275575665" ID="ID_406351009" MODIFIED="1487275581972" TEXT="wei&#xdf;, was zu ihr geh&#xf6;rt"/>
</node>
<node CREATED="1487275584048" FOLDED="true" ID="ID_1846202031" MODIFIED="1488423308115" TEXT="WorkSiteTrail">
<node CREATED="1487275591375" ID="ID_1854949126" MODIFIED="1487275599218" TEXT="verfolgt die WorkSites und ihre Dynamik"/>
</node>
<node CREATED="1487275601373" FOLDED="true" ID="ID_786512805" MODIFIED="1488423308115" TEXT="FocusTracker">
<node CREATED="1487275654958" ID="ID_874086704" MODIFIED="1487275659705" TEXT="verfolgt die Einzelelemente"/>
</node>
</node>
<node CREATED="1487277157451" FOLDED="true" ID="ID_434494653" MODIFIED="1488423342549" TEXT="Teil-Aufgaben">
<node CREATED="1487277161738" ID="ID_400737524" MODIFIED="1487277169981" TEXT="Verlassen einer WorkSite erkennen"/>
<node CREATED="1487277171617" ID="ID_566526238" MODIFIED="1487277189314" TEXT="Ankommen in einer bestehenden WorkSite erkennen"/>
<node CREATED="1487277190310" ID="ID_34185478" MODIFIED="1487277211495" TEXT="Notwendigkeit, eine neue WorkSite zu konstituieren erkennen"/>
</node>
<node CREATED="1487277244375" FOLDED="true" ID="ID_375270405" MODIFIED="1488423342549" TEXT="Ansatz">
<node CREATED="1487277246766" FOLDED="true" ID="ID_747328276" MODIFIED="1488423308116" TEXT="wer?">
<icon BUILTIN="help"/>
<node CREATED="1487277261909" FOLDED="true" ID="ID_551475434" MODIFIED="1488423308102" TEXT="WorkSite">
<node CREATED="1487277266908" ID="ID_1718346792" MODIFIED="1487277271479" TEXT="k&#xf6;nnte das Verlassen erkennen"/>
</node>
<node CREATED="1487277274251" ID="ID_1319104550" MODIFIED="1487277280958" TEXT="WorkSiteTrail"/>
<node CREATED="1487277281882" ID="ID_620509405" MODIFIED="1487277284869" TEXT="FocusTracker"/>
</node>
<node CREATED="1487277256149" FOLDED="true" ID="ID_339253853" MODIFIED="1488423308116" TEXT="wie?">
<icon BUILTIN="help"/>
<node CREATED="1487277291305" ID="ID_1246038804" MODIFIED="1487277306330" TEXT="zentraler Tabellen-Lookup"/>
<node CREATED="1487277307374" ID="ID_335467287" MODIFIED="1487277314153" TEXT="lokalere Member-Tabelle"/>
<node CREATED="1487277317725" ID="ID_381402724" MODIFIED="1487277323744" TEXT="Reverse-Index"/>
</node>
</node>
</node>
<node CREATED="1487277349233" FOLDED="true" ID="ID_1249868383" MODIFIED="1488423342667" TEXT="Resultat">
<node CREATED="1487277352856" ID="ID_587566118" MODIFIED="1487277373993" TEXT="alte WorkSite: verlassen"/>
<node CREATED="1487277375317" ID="ID_977859604" MODIFIED="1487277382904" TEXT="neue WorkSite: aktiviert"/>
<node CREATED="1487277384604" ID="ID_958245251" MODIFIED="1487277397854" TEXT="WorkSiteTrail: aktualisiert"/>
<node CREATED="1487277407425" ID="ID_1880710738" MODIFIED="1487277414275" TEXT="FocusPath: aktualisiert"/>
<node CREATED="1487277414744" ID="ID_1564254807" MODIFIED="1487277429625" TEXT="FocusLocator: hat sich bewegt"/>
<node CREATED="1487277448219" ID="ID_681470264" MODIFIED="1487277485770" TEXT="InteratcionStateManager: benachrichtigt"/>
</node>
</node>
</node>
</node>
<node CREATED="1448070547667" HGAP="16" ID="ID_669457401" MODIFIED="1488423307200" TEXT="Bus-Terminal" VSHIFT="6">
<icon BUILTIN="prepare"/>
<node CREATED="1448070601165" ID="ID_1711137699" MODIFIED="1448070603296" TEXT="Methoden">
<node CREATED="1448407030239" ID="ID_619913132" MODIFIED="1448560681641">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      act, note: Nachricht <i>upstream</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448407054900" ID="ID_1277028437" MODIFIED="1448560688352">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mark: Nachricht <i>downstream</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448560729734" ID="ID_431928237" MODIFIED="1448560743272" TEXT="Nachricht mit Subjekt == indirekt, zur Weiterleitung"/>
<node CREATED="1448560744380" ID="ID_310914031" MODIFIED="1448560762557" TEXT="Nachricht ohne Subjekt == direkt, Subjekt implizit"/>
<node CREATED="1448560790558" ID="ID_1545911136" MODIFIED="1448560800728" TEXT="Bedeutung der Nachricht ergibt sich aus ihrem Namen"/>
</node>
<node CREATED="1448070605948" ID="ID_1779744719" MODIFIED="1448070608272" TEXT="Verhalten">
<node CREATED="1448407128826" ID="ID_1974801494" MODIFIED="1448407138941" TEXT="Bus-Master ist ein speziell implementiertes BusTerm"/>
<node CREATED="1448407139441" ID="ID_392543622" MODIFIED="1450390220709" TEXT="das gew&#xf6;hnliche BusTerm ist einem Tangible zugeordnet"/>
<node CREATED="1448407187035" ID="ID_1622894914" MODIFIED="1448407197917" TEXT="sowie bus-seitig an ein &quot;upstream&quot; BusTerm"/>
</node>
<node CREATED="1448070618434" ID="ID_1925738380" MODIFIED="1448070621878" TEXT="Zuordnung">
<node CREATED="1448406963801" ID="ID_1506114314" MODIFIED="1448406975982">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Bus-Design is <b>selbst&#228;hnlich</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448406977350" ID="ID_1450959387" MODIFIED="1448406993656" TEXT="jedes BusTerm ist lediglich eine personalisierte Bus-Anbindung"/>
<node CREATED="1450390181806" ID="ID_61666430" MODIFIED="1450390266704" TEXT="es registriert beim Bus-Mater einen Link zum zugeh&#xf6;rigen Tangible"/>
<node CREATED="1448406994860" ID="ID_846198635" MODIFIED="1448407022869">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Kennzeichen ist die <b>EntryID</b>&#160;des zugeh&#246;rigen Elements
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1434128059966" ID="ID_823283341" MODIFIED="1488672648352" TEXT="Connect">
<node CREATED="1434128071126" ID="ID_1618124128" MODIFIED="1481413111360" TEXT="UI-Bus">
<linktarget COLOR="#3a8df0" DESTINATION="ID_1618124128" ENDARROW="Default" ENDINCLINATION="-25;-262;" ID="Arrow_ID_539627804" SOURCE="ID_257833497" STARTARROW="Default" STARTINCLINATION="-2;35;"/>
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="prepare"/>
<node CREATED="1448548216556" ID="ID_1971609445" MODIFIED="1448548224236" TEXT="Topologie">
<node CREATED="1448548225659" ID="ID_807917172" MODIFIED="1448548309275" TEXT="upstream = zum Verteiler"/>
<node CREATED="1448548310247" ID="ID_993096699" MODIFIED="1448548316619" TEXT="downstream = zum Empf&#xe4;nger"/>
<node CREATED="1448560840239" ID="ID_1912018032" MODIFIED="1448560846426" TEXT="indirekt = zur Weiterleitung"/>
</node>
<node CREATED="1434128297445" ID="ID_1971555917" MODIFIED="1434128300889" TEXT="Nachrichtenformat">
<node CREATED="1448560865804" ID="ID_907456829" MODIFIED="1448560869319" TEXT="Klassifikation">
<node CREATED="1448548400308" ID="ID_1714197552" MODIFIED="1448548403647" TEXT="dedicated"/>
<node CREATED="1448548404475" ID="ID_1237647034" MODIFIED="1448548407135" TEXT="targetted"/>
<node CREATED="1448548407819" ID="ID_1923087300" MODIFIED="1448548486692" TEXT="flooding"/>
</node>
<node CREATED="1448560887081" ID="ID_1783786781" MODIFIED="1448560890004" TEXT="Addressierung">
<node CREATED="1448560891416" ID="ID_1603035591" MODIFIED="1448560904026" TEXT="per Subjekt"/>
<node CREATED="1448560896232" ID="ID_931223590" MODIFIED="1448560899523" TEXT="implizit"/>
</node>
</node>
<node CREATED="1434128301525" ID="ID_187622243" MODIFIED="1434128303993" TEXT="Parallelit&#xe4;t">
<node CREATED="1450390447884" ID="ID_1176145978" MODIFIED="1450390453590" TEXT="nicht Threadsafe"/>
<node CREATED="1450390454236" ID="ID_1617539841" MODIFIED="1450390469325" TEXT="l&#xe4;uft ausschlie&#xdf;lich im Event-Thrad"/>
</node>
<node CREATED="1434128332277" ID="ID_33025591" MODIFIED="1434128337777" TEXT="Deregistrierung">
<node CREATED="1450390522393" ID="ID_1448879688" MODIFIED="1450390558817" TEXT="Uplink: implizit, h&#xe4;lt Link zu einem Router"/>
<node CREATED="1450390559805" ID="ID_1657985065" MODIFIED="1450390565215" TEXT="Downlink:">
<node CREATED="1450390566252" ID="ID_591456555" MODIFIED="1450390584621" TEXT="registriert eine Route zu einem Tangible"/>
<node CREATED="1450390585329" ID="ID_1371255341" MODIFIED="1450390595419" TEXT="der dtor entfernt diese Route wieder"/>
</node>
<node CREATED="1481413366826" HGAP="34" ID="ID_1180928280" MODIFIED="1488423307202" TEXT="problematisch im Shutdown" VSHIFT="5">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1481413410588" FOLDED="true" ID="ID_1106063275" MODIFIED="1488423342667" TEXT="Shutdown von Zombie-Nodes">
<node CREATED="1481413418027" ID="ID_998027739" MODIFIED="1481413421327" TEXT="reines Test-Problem"/>
<node CREATED="1481413421795" ID="ID_1335417806" MODIFIED="1481413432909" TEXT="gel&#xf6;st durch self-link + tie break"/>
</node>
<node CREATED="1481413436529" FOLDED="true" ID="ID_195971284" MODIFIED="1488423342668" TEXT="Shutdown des CoreService">
<node CREATED="1481413462638" ID="ID_485427993" MODIFIED="1481413469848" TEXT="Problem: Backlink zum Nexus"/>
<node CREATED="1481413470268" ID="ID_688303898" MODIFIED="1481413610394" TEXT="Nexus ist zu dem Zeitpunkt schon destruiert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Die L&#246;sung f&#252;r diese wecheslseitige Abh&#228;ngigkeit
    </p>
    <p>
      ist, den Nexus als Member im CoreService zu haben,
    </p>
    <p>
      weil man dann seine Addresse schon wei&#223;, bevor er erzeugt ist.
    </p>
    <p>
      
    </p>
    <p>
      Dummerweise rettet mich dieser Trick nicht im Shutdown,
    </p>
    <p>
      denn hier nun l&#228;uft tats&#228;chlicher Code aus dem Destruktor heraus!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481413489290" ID="ID_24882673" MODIFIED="1481413519026" TEXT="folglich l&#xe4;uft nur noch die Impl vom BusTerm"/>
<node CREATED="1481413520326" ID="ID_471345968" MODIFIED="1481414008423" TEXT="Endlosschleife (ping-pong)">
<icon BUILTIN="forward"/>
</node>
<node CREATED="1481414009877" FOLDED="true" ID="ID_269084554" MODIFIED="1488423342549" TEXT="...innerer Widerspruch">
<linktarget COLOR="#426895" DESTINATION="ID_269084554" ENDARROW="Default" ENDINCLINATION="36;87;" ID="Arrow_ID_708633830" SOURCE="ID_354799236" STARTARROW="None" STARTINCLINATION="369;0;"/>
<icon BUILTIN="stop-sign"/>
<node CREATED="1481414032482" ID="ID_1121261163" MODIFIED="1481414037813" TEXT="upstream und nicht upstream"/>
<node CREATED="1481414149018" ID="ID_1708781872" MODIFIED="1481414263785" TEXT="kann Reihenfolge nicht etablieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bei einem <i>echten Downstream</i>&#160;k&#246;nnte man daf&#252;r sorgen,
    </p>
    <p>
      da&#223; er grunds&#228;tzlich vor dem Nexus weggeht. Aber nun kommt, auf dem Umweg
    </p>
    <p>
      &#252;ber den Core-Service, der Nexus nach dem Nexus....
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481414420406" FOLDED="true" ID="ID_559357688" MODIFIED="1488423308116" TEXT="erschwerend...">
<icon BUILTIN="smily_bad"/>
<node CREATED="1481414434148" ID="ID_333480677" MODIFIED="1481414440176" TEXT="BusTerm ist stateless"/>
<node CREATED="1481414477766" ID="ID_695637715" MODIFIED="1481414504287" TEXT="soll auch so bleiben">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ich will nicht damit anfangen, da&#223; man einen Zeiger umsetzen kann....
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1481415983533" FOLDED="true" HGAP="37" ID="ID_1500277801" MODIFIED="1488423342668" TEXT="Trick l&#xf6;st beide" VSHIFT="6">
<icon BUILTIN="button_ok"/>
<node CREATED="1481415996732" ID="ID_98181285" MODIFIED="1481416003799" TEXT="ein &quot;circuit breaker&quot;"/>
<node CREATED="1481416013842" ID="ID_1905276259" MODIFIED="1481416044386">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      beendet Deregistrierung,
    </p>
    <p>
      wenn ein&#160;BusTerm sich selbst deregistriert
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1481416047509" ID="ID_1445652228" MODIFIED="1481416060327" TEXT="etwas halbseiden"/>
<node CREATED="1481416060843" ID="ID_354799236" MODIFIED="1481416128424" TEXT="aber... ein innerer Widerspruch liegt zugrunde">
<arrowlink COLOR="#426895" DESTINATION="ID_269084554" ENDARROW="Default" ENDINCLINATION="36;87;" ID="Arrow_ID_708633830" STARTARROW="None" STARTINCLINATION="369;0;"/>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1434128310005" ID="ID_644247390" MODIFIED="1434128318561" TEXT="Knoten-ID">
<node CREATED="1450390501876" ID="ID_1750345192" MODIFIED="1450390509511" TEXT="ist die EntryID des Tangible"/>
<node CREATED="1450390512635" ID="ID_43739373" MODIFIED="1450390519885" TEXT="dient auch zum Routing"/>
</node>
<node CREATED="1453546261390" HGAP="25" ID="ID_1996322416" MODIFIED="1453546322869" TEXT="Test" VSHIFT="12">
<node CREATED="1453546264638" ID="ID_975821244" MODIFIED="1455421865474" TEXT="Test-Nexus">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1453546268998" ID="ID_1692848170" MODIFIED="1487275491136" TEXT="Event-Log">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Mechanismus, der es erlaubt
    </p>
    <ul>
      <li>
        log-Nachrichten aus Mocks zu hinterlassen
      </li>
      <li>
        in der Test-Fixture auf diese zu matchen
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485548830035" HGAP="5" ID="ID_1295711115" MODIFIED="1485548844056" TEXT="Nutzen" VSHIFT="17">
<icon BUILTIN="bell"/>
<node CREATED="1485548856639" ID="ID_1677281474" MODIFIED="1485548861522" TEXT="spezifische Aktionen">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485548894522" FOLDED="true" ID="ID_826011549" MODIFIED="1488672334405" TEXT="Problem: globale Aktionen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Beispiel: Aktionen, die im globalen Men&#252; stehen.
    </p>
    <p>
      &quot;Add Sequence&quot;
    </p>
    <ul>
      <li>
        wer bildet daraus ein Command?
      </li>
      <li>
        auf welchen Kontext bezieht sich das
      </li>
      <li>
        wen kann die Men&#252;-Registrierung konkret ansprechen (Verdrahtung ist statisch)
      </li>
    </ul>
  </body>
</html></richcontent>
<linktarget COLOR="#824f68" DESTINATION="ID_826011549" ENDARROW="Default" ENDINCLINATION="-906;-1187;" ID="Arrow_ID_1373278707" SOURCE="ID_1122969153" STARTARROW="Default" STARTINCLINATION="870;255;"/>
<icon BUILTIN="messagebox_warning"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1485555902731" FOLDED="true" ID="ID_1913236669" MODIFIED="1488423342549" TEXT="#1070 how to bind session commands into UI actions">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1486767998242" ID="ID_1898269374" MODIFIED="1486768004600" TEXT="InteractionDirector verwenden">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
</node>
<node CREATED="1485548866990" ID="ID_664933854" MODIFIED="1485548877800" TEXT="Commands"/>
<node CREATED="1485548878292" ID="ID_49934573" MODIFIED="1485548882087" TEXT="Broadcast"/>
</node>
</node>
<node CREATED="1453545812389" HGAP="43" ID="ID_202253849" MODIFIED="1487275491177" TEXT="Core-Services" VSHIFT="2">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      konzeptionell: fertig
    </p>
    <p>
      Implementierung der real-world-Variante fehlt!
    </p>
  </body>
</html></richcontent>
<arrowlink DESTINATION="ID_1343270939" ENDARROW="Default" ENDINCLINATION="-197;638;" ID="Arrow_ID_1686519160" STARTARROW="Default" STARTINCLINATION="-83;-1020;"/>
<font NAME="SansSerif" SIZE="13"/>
<icon BUILTIN="prepare"/>
<node CREATED="1453545875627" ID="ID_1411740156" MODIFIED="1487275491186" TEXT="Definition &#xbb;Zentral-Dienste&#xab;">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Dienste im UI, erreichbar &#252;ber den Bus.
    </p>
    <p>
      Sie stellen die Verbindung zu zentralen Belangen her
    </p>
    <p>
      wie Session- und State-Managment, Commands etc.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1453545968166" ID="ID_399884859" MODIFIED="1483927661023" TEXT="Command-Handler">
<icon BUILTIN="button_ok"/>
<node CREATED="1453546129537" ID="ID_1926242869" MODIFIED="1488423307204" TEXT="empf&#xe4;ngt &quot;act&quot;-Nachrichten">
<icon BUILTIN="button_ok"/>
<node CREATED="1453546367201" ID="ID_1473684366" MODIFIED="1453546575633" TEXT="&quot;bang&quot; -&gt; HandlingPattern">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1453546381255" FOLDED="true" ID="ID_1252400934" MODIFIED="1488423342668" TEXT="&quot;bind&quot; -&gt; Bindung">
<icon BUILTIN="button_ok"/>
<node CREATED="1453546393573" ID="ID_857178151" MODIFIED="1453546568844" TEXT="Paradigmen-Mismatch">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1453546417098" ID="ID_676713402" MODIFIED="1453546563455" TEXT="DOM vs. compiletime typing">
<icon BUILTIN="info"/>
</node>
<node CREATED="1453546436455" FOLDED="true" ID="ID_129001401" MODIFIED="1488423342550" TEXT="double-dispatch">
<icon BUILTIN="idea"/>
<node CREATED="1453546450317" FOLDED="true" ID="ID_1899026394" MODIFIED="1488423308116" TEXT="Variant-Visitor">
<icon BUILTIN="button_ok"/>
<node CREATED="1453546485737" ID="ID_491338311" MODIFIED="1453546493300" TEXT="Gen-Node-Typen"/>
<node CREATED="1453546493719" ID="ID_834270776" MODIFIED="1453546496947" TEXT="fest vorgegeben"/>
</node>
<node CREATED="1453546454484" ID="ID_1643300093" MODIFIED="1453590507167" TEXT="Trampolin">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1453546512277" ID="ID_968193365" MODIFIED="1453590559627" TEXT="Aufruf: generische Iteration">
<icon BUILTIN="info"/>
</node>
<node CREATED="1453590522161" FOLDED="true" ID="ID_1369411965" MODIFIED="1488423308116" TEXT="Problem: narrowing conversions">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1453590585865" ID="ID_350595711" MODIFIED="1453590606670" TEXT="explizit gef&#xe4;hrliche ausschlie&#xdf;en">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1453638973049" ID="ID_1350801390" LINK="https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63723" MODIFIED="1453639023645">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Compiler-Bug <font color="#c80219">Gcc-#63723</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1453639033250" ID="ID_1669831951" LINK="http://stackoverflow.com/questions/26705199/how-can-i-write-a-type-trait-to-check-if-a-type-is-convertible-to-another-by-a-n" MODIFIED="1453639049952" TEXT="s.a. Stackoverflow"/>
<node CREATED="1453639055478" ID="ID_1560466275" MODIFIED="1453639078468">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gel&#246;st in GCC-5 --&#160;<i>backport unwahrscheinlich</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1453639119142" ID="ID_1472487058" MODIFIED="1453639126177" TEXT="inzwischen hartgecodet">
<icon BUILTIN="clanbomber"/>
</node>
</node>
<node CREATED="1453546467707" FOLDED="true" ID="ID_1543497504" MODIFIED="1488423308116" TEXT="Problem: template bloat">
<node CREATED="1453590622236" FOLDED="true" ID="ID_1084684065" MODIFIED="1488423308102" TEXT="h&#xe4;lt sich in Grenzen">
<icon BUILTIN="ksmiletris"/>
<node CREATED="1453590828976" ID="ID_659078492" MODIFIED="1453590855465" TEXT="eine VTable pro Zieltyp"/>
<node CREATED="1453590856220" ID="ID_376478329" MODIFIED="1453590879683">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eine virtuelle Funktion
    </p>
    <p>
      pro m&#246;glichem Umwandlungs-Pfad
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1453590628419" ID="ID_1314031268" MODIFIED="1453590885601">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wir verwenden die Basis-VTable
    </p>
    <p>
      und layern nur die tats&#228;chlich m&#246;glichen Umwandlungen dr&#252;ber
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
<node CREATED="1453590667630" ID="ID_792450888" MODIFIED="1453590699782" TEXT="&#xfc;berpr&#xfc;ft: mit -O3 und strip bleibt erstaunlich wenig &#xfc;brig"/>
<node CREATED="1453590741428" ID="ID_1733557933" MODIFIED="1453590749327" TEXT="1.6MiB -&gt; 100k"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1483925280108" FOLDED="true" HGAP="30" ID="ID_1479669922" MODIFIED="1488423342668" TEXT="Protokoll-Erweiterung" VSHIFT="8">
<arrowlink COLOR="#e9ba2d" DESTINATION="ID_242219866" ENDARROW="Default" ENDINCLINATION="-784;1692;" ID="Arrow_ID_1617518648" STARTARROW="None" STARTINCLINATION="3436;-1136;"/>
<icon BUILTIN="help"/>
<node CREATED="1483925298713" ID="ID_347702585" MODIFIED="1483925306749" TEXT="Command-Prototyp forken"/>
<node CREATED="1483925307385" ID="ID_509982277" MODIFIED="1483925311132" TEXT="neue ID generieren lassen"/>
<node CREATED="1483925311624" ID="ID_1592410347" MODIFIED="1483925338280" TEXT="Anforderung vom InvocationStateManager"/>
<node CREATED="1483925338796" ID="ID_348930186" MODIFIED="1483925348375" TEXT="implementiert in SessionCommandService"/>
</node>
</node>
<node CREATED="1453546138215" ID="ID_621186206" MODIFIED="1488423307209" TEXT="Verbindung zum ProcDispatcher">
<icon BUILTIN="button_ok"/>
<node CREATED="1483927603312" ID="ID_1803909975" MODIFIED="1483927622749" TEXT="Aufruf der SessionCommand facade">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483927624157" ID="ID_86978128" MODIFIED="1483927636406" TEXT="in CommandHandler gepackt">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1483927637022" ID="ID_455471953" MODIFIED="1483927642261" TEXT="total einfach hier">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1453546183561" ID="ID_1080090125" MODIFIED="1453546187900" TEXT="Mock-Variante">
<node CREATED="1453546195240" ID="ID_1905175534" MODIFIED="1454711242234" TEXT="loggen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1453546197727" ID="ID_606855270" MODIFIED="1454711245274" TEXT="verifizierbar machen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1453546215117" ID="ID_31965862" MODIFIED="1453546220236" TEXT="pseudo-Command">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1483927678318" HGAP="25" ID="ID_1324099152" MODIFIED="1483927692050" TEXT="Notification-Facade &#xf6;ffnen" VSHIFT="4">
<icon BUILTIN="flag-yellow"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1483927692988" ID="ID_354422174" MODIFIED="1483927756533" TEXT="Lebenszyklus-Fragen verifizieren">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1483927735679" ID="ID_783490676" MODIFIED="1483927757596" TEXT="Integration in UI-Main verifizieren">
<icon BUILTIN="flag-yellow"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1483927710450" ID="ID_1085939201" MODIFIED="1483927759044" TEXT="Hochfahren des UI / Schlie&#xdf;en des UI">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1453546083296" ID="ID_1154674875" MODIFIED="1455421793848" TEXT="Presentation-State-Manager">
<icon BUILTIN="button_ok"/>
<node CREATED="1455290707481" ID="ID_551153117" MODIFIED="1455290724712">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      empf&#228;ngt alle <b>state mark notificatons</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455290726358" ID="ID_386120755" MODIFIED="1455290731577" TEXT="mu&#xdf; gruppieren">
<node CREATED="1455290732805" ID="ID_1731455164" MODIFIED="1455290737616" TEXT="nach Ui-Element"/>
<node CREATED="1455290738044" ID="ID_1309710182" MODIFIED="1455290745959" TEXT="nach property innerhalb"/>
</node>
<node CREATED="1455290748539" ID="ID_504317246" MODIFIED="1455290870527" TEXT="zeichnet jeweils den letzten aktuellen Zustand auf">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1455290768888" ID="ID_1779151953" MODIFIED="1455290865016" TEXT="Wechselwirkungen">
<icon BUILTIN="info"/>
<node CREATED="1455290773936" ID="ID_533210684" MODIFIED="1455290798348" TEXT="geplant, absehbar">
<icon BUILTIN="bell"/>
</node>
<node CREATED="1455290807459" ID="ID_1222290895" MODIFIED="1455290825988" TEXT="mu&#xdf; Ausz&#xfc;ge aus dieser Info schaffen"/>
<node CREATED="1455290827193" ID="ID_1429808095" MODIFIED="1455290846062">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nach <b>Perspektive</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455290833048" ID="ID_1650274766" MODIFIED="1455290841159">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nach <b>work site</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1455290849374" ID="ID_906623432" MODIFIED="1455290855766" TEXT="future work">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1488672534372" HGAP="78" ID="ID_1920838120" MODIFIED="1488672850503" TEXT="core commands" VSHIFT="21">
<icon BUILTIN="stop"/>
<node CREATED="1488672580926" ID="ID_1920974710" MODIFIED="1488672584130" TEXT="konzeptionell">
<node CREATED="1488672621585" ID="ID_1461875887" MODIFIED="1488672817699" TEXT="Command-cycle">
<linktarget COLOR="#706a9a" DESTINATION="ID_1461875887" ENDARROW="Default" ENDINCLINATION="-923;-789;" ID="Arrow_ID_457540370" SOURCE="ID_969011841" STARTARROW="None" STARTINCLINATION="806;300;"/>
<icon BUILTIN="pencil"/>
<node CREATED="1488672838499" ID="ID_1617972329" MODIFIED="1488672841439" TEXT="Interaktionen">
<node CREATED="1488672913049" ID="ID_1455665748" MODIFIED="1488672923667" TEXT="brauche InvocationTrail">
<node CREATED="1488675922636" ID="ID_951339978" MODIFIED="1488676038538" TEXT="kann man subsumieren">
<arrowlink COLOR="#72a0f5" DESTINATION="ID_329010871" ENDARROW="Default" ENDINCLINATION="-16;-107;" ID="Arrow_ID_1387738585" STARTARROW="None" STARTINCLINATION="-50;-26;"/>
<icon BUILTIN="yes"/>
</node>
<node CREATED="1488675931163" ID="ID_1990262471" MODIFIED="1488675953207" TEXT="InteractionStateManager == Abstraktion">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1488672957922" ID="ID_329010871" MODIFIED="1488676593469" TEXT="brauche InteractionStateManager">
<arrowlink COLOR="#5a80c1" DESTINATION="ID_121485208" ENDARROW="Default" ENDINCLINATION="9;-164;" ID="Arrow_ID_17404383" STARTARROW="None" STARTINCLINATION="1;52;"/>
<linktarget COLOR="#72a0f5" DESTINATION="ID_329010871" ENDARROW="Default" ENDINCLINATION="-16;-107;" ID="Arrow_ID_1387738585" SOURCE="ID_951339978" STARTARROW="None" STARTINCLINATION="-50;-26;"/>
<node CREATED="1488676089349" ID="ID_1584585045" MODIFIED="1488676118181" TEXT="Anfrage ...f&#xfc;r ein bestimmes Command"/>
<node CREATED="1488676629451" ID="ID_989334768" MODIFIED="1488676647900" TEXT="Problem: wie findet man den Zust&#xe4;ndigen">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1488676134951" ID="ID_1426727929" MODIFIED="1488676142770" TEXT="N : M">
<node CREATED="1488676119929" ID="ID_70646733" MODIFIED="1488676172343">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      einer k&#246;nnte f&#252;r
    </p>
    <p>
      mehrere Commands zust&#228;ndig sein
    </p>
  </body>
</html></richcontent>
<node CREATED="1488676210948" ID="ID_1003704499" MODIFIED="1488676230046" TEXT="verschiedene Auspr&#xe4;gungen"/>
<node CREATED="1488676230802" ID="ID_1711345566" MODIFIED="1488676234293" TEXT="generische Situation"/>
</node>
<node CREATED="1488676179840" ID="ID_1207866065" MODIFIED="1488676200726">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mehrere k&#246;nnten f&#252;r
    </p>
    <p>
      ein Command zust&#228;ndig sein
    </p>
  </body>
</html></richcontent>
<node CREATED="1488676238120" ID="ID_62079120" MODIFIED="1488676259969" TEXT="da an ein Control/Interface-System gebunden"/>
<node CREATED="1488676280459" ID="ID_800780235" MODIFIED="1488676340897" TEXT="ist vermutlich keine gute Idee">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....denn wir wollen ja grade
    </p>
    <p>
      den Widget-Code vom Control-System abstrahieren
    </p>
    <p>
      und ebenso die Gesten abstrahieren
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1488676343170" ID="ID_1624806724" MODIFIED="1488676361392" TEXT="besser? InteractionStateManager == Geste">
<icon BUILTIN="help"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1488674244218" ID="ID_121485208" MODIFIED="1488676593469" TEXT="Aktivit&#xe4;ten">
<linktarget COLOR="#5a80c1" DESTINATION="ID_121485208" ENDARROW="Default" ENDINCLINATION="9;-164;" ID="Arrow_ID_17404383" SOURCE="ID_329010871" STARTARROW="None" STARTINCLINATION="1;52;"/>
<node CREATED="1488674249665" ID="ID_1028887910" MODIFIED="1488674254196" TEXT="Command-Instanz">
<node CREATED="1488674520524" ID="ID_895059689" MODIFIED="1488674526217" TEXT="Prototyp-Pattern">
<icon BUILTIN="info"/>
</node>
<node CREATED="1488674528035" ID="ID_1084922722" MODIFIED="1488674610778">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>mu&#223;</i>&#160;Instanzen einsetzen
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und zwar zwingend, sobald
    </p>
    <ul>
      <li>
        das command Argument hat, die gebunden sein wollen
      </li>
      <li>
        mehrere Invocations des gleichen Grund-Commands &quot;gleichzeitig&quot; unterwegs sein k&#246;nnten
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1488674659521" ID="ID_1283697108" MODIFIED="1488674673114" TEXT="Problem: Instanz-Management">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1488674675895" ID="ID_1414738474" MODIFIED="1488674746218" TEXT="benannt oder anonyom">
<icon BUILTIN="help"/>
</node>
<node CREATED="1488674682869" ID="ID_1865712804" MODIFIED="1488674755779" TEXT="anonym == ref counted">
<icon BUILTIN="info"/>
</node>
<node CREATED="1488674692293" ID="ID_1154381764" MODIFIED="1488674711644" TEXT="bindet uns an Proc">
<icon BUILTIN="stop-sign"/>
<node CREATED="1488674807716" ID="ID_1220416942" MODIFIED="1488674814949" TEXT="ist das ein Problem?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1488674815867" ID="ID_417989611" MODIFIED="1488674903230" TEXT="eigentlich nicht">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...vom Linken her nicht, da wir Gui gegen Proc linken
    </p>
    <p>
      vom Bauen auch nicht, und au&#223;erdem...
    </p>
    <p>
      ...coden wir ja nicht gegen die Implementierung,
    </p>
    <p>
      sondern gegen eine Abstraction (Command), die eigens daf&#252;r geschaffen wurde
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1488674905160" ID="ID_1688143285" MODIFIED="1488674916271" TEXT="ungutes Gef&#xfc;hl">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="smily_bad"/>
<node CREATED="1488674941243" ID="ID_1628081916" MODIFIED="1488674949133" TEXT="unterl&#xe4;uft das Interface-System"/>
<node CREATED="1488675139160" ID="ID_1466153465" MODIFIED="1488675146082" TEXT="unterl&#xe4;uft das Messaging / UI-Bus"/>
</node>
</node>
<node CREATED="1488674721265" ID="ID_1409487095" MODIFIED="1488674735629" TEXT="aber: benannte Instanzen leben einfach weiter">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1488677059648" ID="ID_1869818470" MODIFIED="1488677072562" TEXT="war eigentlich nur f&#xfc;r &quot;Spezialisierungen&quot; gedacht"/>
<node CREATED="1488936223175" ID="ID_1105962999" MODIFIED="1488936233654" TEXT="zu pr&#xfc;fen: overhead">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1488936242413" ID="ID_1255236132" MODIFIED="1488936261039" TEXT="k&#xf6;nnte eigener, dritter Typus werden">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1488936634128" ID="ID_857930488" MODIFIED="1488936700939" TEXT="Konsequenz">
<icon BUILTIN="button_ok"/>
<node CREATED="1488936638247" ID="ID_699367403" MODIFIED="1488936651353" TEXT="man sollte auf das Bauchgef&#xfc;hl h&#xf6;ren"/>
<node CREATED="1488936653341" ID="ID_629143054" MODIFIED="1488936669310" TEXT="Design leitet in richtige Bahnen"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488936670970" ID="ID_697148715" MODIFIED="1488936742194" TEXT="die Trennung respektieren">
<arrowlink COLOR="#79d29b" DESTINATION="ID_1131387570" ENDARROW="Default" ENDINCLINATION="-48;-109;" ID="Arrow_ID_1598234794" STARTARROW="None" STARTINCLINATION="-421;-27;"/>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488675785223" ID="ID_134788782" MODIFIED="1488675850639" TEXT="Aufgaben">
<icon BUILTIN="flag-yellow"/>
<node COLOR="#338800" CREATED="1488675788278" ID="ID_241828684" MODIFIED="1488936420989" TEXT="wer erzeugt die Instanz?">
<icon BUILTIN="button_ok"/>
<node CREATED="1488676735628" ID="ID_845338567" MODIFIED="1488936413573" TEXT="der InteractionStateManager">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1488676746667" ID="ID_1675443482" MODIFIED="1488936417954" TEXT="jemand in Proc">
<icon BUILTIN="button_ok"/>
<node CREATED="1488936425628" ID="ID_273815265" MODIFIED="1488936428431" TEXT="neue Entit&#xe4;t"/>
<node CREATED="1488936429139" ID="ID_858892843" MODIFIED="1488936433830" TEXT="geh&#xf6;rt zum Command-System"/>
</node>
</node>
<node CREATED="1488675795485" ID="ID_560742641" MODIFIED="1488936760622" TEXT="wer benennt die Instanz?">
<icon BUILTIN="help"/>
<node CREATED="1488676790165" ID="ID_450420022" MODIFIED="1488676795858" TEXT="der der sie erzeugt">
<icon BUILTIN="help"/>
</node>
<node CREATED="1488676796892" ID="ID_399980219" MODIFIED="1488676813292" TEXT="ein Benennungs-Service in Proc">
<icon BUILTIN="help"/>
</node>
<node CREATED="1488676805491" ID="ID_1995019439" MODIFIED="1488676815444" TEXT="ein Instanz-Pool">
<icon BUILTIN="help"/>
</node>
</node>
<node COLOR="#338800" CREATED="1488675800668" ID="ID_260870361" MODIFIED="1488936768854" TEXT="wer h&#xe4;lt die Instanz (am Leben)?">
<icon BUILTIN="button_ok"/>
<node CREATED="1488676827432" ID="ID_1354771185" MODIFIED="1488676881245" TEXT="Problem: air-Gap">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wegen
    </p>
    <ul>
      <li>
        Command-Message via UI-Bus
      </li>
      <li>
        Durchreichen durch das Interface-System
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#ff0000" CREATED="1488677586744" ID="ID_1077384395" MODIFIED="1488677612409" TEXT="AUA: Zwang zum Benennen">
<icon BUILTIN="stop-sign"/>
</node>
</node>
<node CREATED="1488676941240" ID="ID_403164604" MODIFIED="1488676965416" TEXT="im Proc-Dispatcher kein Problem">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...da die DispatcherQueue direkt Command-Objekte (=frontend handle) speichert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1488677212907" ID="ID_1525612446" MODIFIED="1488936784410" TEXT="Idee: im InteractionStateManager">
<icon BUILTIN="button_cancel"/>
<node CREATED="1488677232640" ID="ID_1033002023" MODIFIED="1488677274454" TEXT="null oder maximal eine pro Command"/>
<node CREATED="1488677258932" ID="ID_1695115228" MODIFIED="1488677268487" TEXT="andere Geste -&gt; andere Command-Instanz"/>
<node COLOR="#c15a5d" CREATED="1488677333066" ID="ID_182292267" MODIFIED="1488677344667" TEXT="Aber">
<node CREATED="1488677346473" ID="ID_712110821" MODIFIED="1488677350100" TEXT="air-Gap"/>
<node CREATED="1488677350640" ID="ID_901346570" MODIFIED="1488677370377" TEXT="Ausf&#xfc;hrung im Dispatcher erst sp&#xe4;ter"/>
<node COLOR="#338800" CREATED="1488677418087" ID="ID_644949863" MODIFIED="1488677499488" TEXT="ist doch kein Problem">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn das GUI l&#228;uft ja synchron.
    </p>
    <p>
      D.h. wir wissen, wenn wir das Air-Gap &#252;berstanden haben.
    </p>
    <p>
      Ab diesem Punkt h&#228;lt der Eintrag in der DispatcherQueue das Command am Leben,
    </p>
    <p>
      und wenn es stirbt, dann stirbt es halt...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1488936070394" ID="ID_1131387570" MODIFIED="1488936736607" TEXT="Schlu&#xdf;folgerung">
<linktarget COLOR="#79d29b" DESTINATION="ID_1131387570" ENDARROW="Default" ENDINCLINATION="-48;-109;" ID="Arrow_ID_1598234794" SOURCE="ID_697148715" STARTARROW="None" STARTINCLINATION="-421;-27;"/>
<icon BUILTIN="forward"/>
<node CREATED="1488936102272" ID="ID_414025307" MODIFIED="1488936125740" TEXT="eigenes Instantiierungs-Protokoll">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1488936115534" ID="ID_1508575123" MODIFIED="1488936128074" TEXT="explizit in Proc zu unterst&#xfc;tzen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1488936129180" ID="ID_62638556" MODIFIED="1488936137127" TEXT="hand-over">
<node CREATED="1488936140275" ID="ID_61978297" MODIFIED="1488936152237" TEXT="entfernt Registrierung"/>
<node CREATED="1488936152849" ID="ID_525023498" MODIFIED="1488936164963" TEXT="erzeugt anonyme Instanz"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1488936910756" HGAP="200" ID="ID_208733336" MODIFIED="1488936924589" TEXT="neues Design" VSHIFT="14">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1488937072267" ID="ID_1765750778" MODIFIED="1488937082039" TEXT="separation of concerns">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1488936925448" ID="ID_1949523361" MODIFIED="1488937085712" TEXT="zwei neue Entit&#xe4;ten">
<icon BUILTIN="yes"/>
<node CREATED="1488936938742" ID="ID_1388901840" MODIFIED="1488939645006">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      GUI: <b>CmdAccessor</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1488936962690" ID="ID_399848637" MODIFIED="1488936995228" TEXT="vermittelt InvocationStateManager"/>
<node CREATED="1488937001406" ID="ID_1747520920" MODIFIED="1488937012304" TEXT="letzterer ist Interface f&#xfc;r UI-Entit&#xe4;ten"/>
</node>
<node CREATED="1488936942317" ID="ID_222608258" MODIFIED="1488939548823">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Proc: <b>CmdInstanceManager</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1488937028202" ID="ID_1121930542" MODIFIED="1488937033253" TEXT="erzeugt die Instanzen"/>
<node CREATED="1488937033769" ID="ID_1795817178" MODIFIED="1488937045947" TEXT="verwaltet deren Lebenszyklus"/>
<node CREATED="1488937046591" ID="ID_1911259949" MODIFIED="1488937055426" TEXT="vom UI allein per ID ansprechbar"/>
</node>
</node>
<node CREATED="1488938186100" ID="ID_98580620" MODIFIED="1489191362863" TEXT="Command-Definition erweitern">
<icon BUILTIN="hourglass"/>
<node CREATED="1488938195387" ID="ID_1831459518" MODIFIED="1488938202950" TEXT="M&#xf6;glichkeit, Regeln vorzugeben"/>
<node CREATED="1488938222135" ID="ID_1365861102" MODIFIED="1488938232250" TEXT="werden fest mit dem Command zusammen konfiguriert"/>
<node CREATED="1488938205266" ID="ID_1560120611" MODIFIED="1488938215411" TEXT="diese k&#xf6;nnen aus der Command-Def bezogen werden"/>
</node>
<node CREATED="1488939658004" ID="ID_726054321" MODIFIED="1488939668212" TEXT="Interaktionen">
<icon BUILTIN="info"/>
<node CREATED="1488939675705" ID="ID_1515081819" MODIFIED="1488939724134" TEXT="UI-Element fragt CmdAccessor nach InteractionStateManager"/>
<node CREATED="1488939946828" ID="ID_1817986480" MODIFIED="1488940966027" TEXT="CmdAccessor speichert InteractionStateManager"/>
<node CREATED="1488939748247" ID="ID_589914539" MODIFIED="1489079785598" TEXT="InteractionStateManager belegt beim CmdInstanceManager neue ID..."/>
<node CREATED="1488940951052" ID="ID_1899882798" MODIFIED="1488940964629" TEXT="...und erzeugt daraus neuen InvocationTrail"/>
<node CREATED="1488940222591" ID="ID_1071611814" MODIFIED="1488940241768" TEXT="UI-Element fragt InteractionStateManager: &quot;kann dieses Command ausgef&#xfc;hrt werden&quot;?"/>
<node CREATED="1488940254106" ID="ID_1373320189" MODIFIED="1488940276539" TEXT="UI-Element bezieht vom InteractionStateManager einen InvocationTrail"/>
<node CREATED="1488940300116" ID="ID_172216455" MODIFIED="1488940331244" TEXT="UI-Element registriert sich beim InteractionStateManager (als Observer)"/>
<node CREATED="1488940435601" ID="ID_1906295137" MODIFIED="1488940468681" TEXT="UI-Element verwendet InvocationTrail, um Command zu triggern"/>
<node CREATED="1488940469365" ID="ID_130562988" MODIFIED="1488940518450" TEXT="CmdInstanceManager &#xfc;bergibt (move) die Instanz an den ProcDispatcher"/>
<node CREATED="1488940519518" ID="ID_1875791797" MODIFIED="1488940533992" TEXT="bei n&#xe4;chster Anfrage wird CmdInstanceManager daher neue Instanz anlegen"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1488677859619" HGAP="-15" ID="ID_827179653" MODIFIED="1489191299696" TEXT="Problem: Service-Zugang" VSHIFT="24">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...da dies ein pervasiv genutzter Service ist,
    </p>
    <p>
      und wir nicht wollen, da&#223; jedes Widget
    </p>
    <p>
      mit dem InteractionDirector verdrahtet sein mu&#223;!
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#9f5680" DESTINATION="ID_850989325" ENDARROW="Default" ENDINCLINATION="211;189;" ID="Arrow_ID_1007188680" STARTARROW="None" STARTINCLINATION="912;-974;"/>
<linktarget COLOR="#758ba4" DESTINATION="ID_827179653" ENDARROW="Default" ENDINCLINATION="173;83;" ID="Arrow_ID_57938013" SOURCE="ID_1621107057" STARTARROW="None" STARTINCLINATION="550;-93;"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1488936835940" ID="ID_1322149090" MODIFIED="1488937388772" TEXT="separates Problem">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hat &#252;berhaupt nichts mit dem Zugang zu Commands zu tun,
    </p>
    <p>
      und auch nichts mit der Trennung zwischen Layern und Subsystemen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1488936840059" ID="ID_1305671938" MODIFIED="1488937417519" TEXT="generisches Problem">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es geht um Service-Dependencies
    </p>
    <p>
      aka DependencyInjection + Lifecycle Management
    </p>
  </body>
</html></richcontent>
<node CREATED="1488937419308" ID="ID_994813744" MODIFIED="1488937424552" TEXT="lib::Depend ausbauen"/>
<node CREATED="1488937436890" ID="ID_1184288068" MODIFIED="1488937442437" TEXT="kann auf sp&#xe4;ter vertagt werden"/>
<node CREATED="1488937424996" ID="ID_1712367280" MODIFIED="1488937434311" TEXT="vorerst eigenes Front-End verwenden"/>
<node CREATED="1488937449896" ID="ID_1352702920" MODIFIED="1488937496324" TEXT="Idee klar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <ul>
      <li>
        man hat ein statisches Front-End, d.h. by-name access
      </li>
      <li>
        hinter dem liegt eine Factory
      </li>
      <li>
        die Instanz kann von innen her wieder geschlossen werden
      </li>
      <li>
        wenn geschlossen, dann Fehler werfen
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1488674268407" ID="ID_284516869" MODIFIED="1488674271114" TEXT="Parameter">
<node CREATED="1488937542963" ID="ID_853531623" MODIFIED="1488937550998" TEXT="passiert im InvocationStateManager"/>
<node CREATED="1488937551514" ID="ID_1024843173" MODIFIED="1488937557157" TEXT="es k&#xf6;nnte Binde-Regeln geben"/>
<node CREATED="1488937996901" ID="ID_1000532365" MODIFIED="1488940083440" TEXT="L&#xf6;sungsweg vom Command vorkonfiguriert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...nicht klar, ob das notwendig (und gut) ist
    </p>
    <p>
      es k&#246;nnte auch ausreichen, einfach die passende InteractionStateManager-Impl zu verwenden
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1488937565632" ID="ID_1990212985" MODIFIED="1488940116992" TEXT="dem User (UI-Element) ist das egal">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      denn InteractionStateManager ist ein <b>Interface</b>!
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1488937651021" ID="ID_1956085726" MODIFIED="1488937977999" TEXT="Binde-Muster">
<icon BUILTIN="idea"/>
<node CREATED="1488937670778" ID="ID_696645875" MODIFIED="1488937783542" TEXT="fest hinterlegte Eintit&#xe4;t einsetzen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das UI wei&#223;,
    </p>
    <p>
      wer das konkret immer sein wird.
    </p>
    <p>
      D.h. beim Start des UI wird eine Verbindung irgendwo hinterlegt
    </p>
    <p>
      Das k&#246;nnte ein <i>Advice</i>&#160;sein
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1488937682649" ID="ID_1090053275" MODIFIED="1488937855797" TEXT="aktuelles Element (mit Typ-Check)">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vom Command her ist der Typ festgelegt
    </p>
    <p>
      auf das &quot;aktuelle Element&quot; wir eine Art Typ-Match gemacht.
    </p>
    <p>
      Wenn der pa&#223;t, kann das aktuelle Element verwendet werden.
    </p>
    <p>
      In diesem Fall wird das Command <i>enabled</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1488937711053" ID="ID_586793367" MODIFIED="1488937890369" TEXT="partial application">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eine Argumentliste mit mehreren Parametern wir Schritt f&#252;r Schritt geschlossen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1488937724867" ID="ID_231519502" MODIFIED="1488937932885" TEXT="Ordnung nach Scope">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn mehrere Objekte als Argumente in Frage kommen,
    </p>
    <p>
      wird das gem&#228;&#223; Scope &quot;n&#228;chstgelegne&quot; genommen
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1488937933566" ID="ID_170827087" MODIFIED="1488937945712" TEXT="das sind komplexe Pl&#xe4;ne">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1488937947004" ID="ID_1297081811" MODIFIED="1488937965632" TEXT="Schlu&#xdf;folgerung: nur die Strategie vorsehen">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1488672976664" ID="ID_476262016" MODIFIED="1488672983866" TEXT="Rollen">
<node CREATED="1488673008668" ID="ID_1231961413" MODIFIED="1488673044658" TEXT="InvocationTrail">
<node CREATED="1488673045793" ID="ID_969627518" MODIFIED="1488673045793" TEXT="passiver Marker f&#xfc;r eine Instanz"/>
</node>
<node CREATED="1488673032384" ID="ID_1621941781" MODIFIED="1488673052737" TEXT="InteractionStateManager">
<node CREATED="1488673053917" ID="ID_312201027" MODIFIED="1488673056808" TEXT="Vermittler"/>
<node CREATED="1488673066524" ID="ID_1129594057" MODIFIED="1488673069415" TEXT="Bildung von S&#xe4;tzen"/>
</node>
</node>
</node>
<node CREATED="1488672585310" ID="ID_50525010" MODIFIED="1488672593368" TEXT="framework">
<node CREATED="1489191062441" ID="ID_772085108" MODIFIED="1489191064789" TEXT="Aktoren">
<node CREATED="1489191072415" ID="ID_546981226" MODIFIED="1489191086697" TEXT="Command Definition"/>
<node CREATED="1489191088613" ID="ID_1858363588" MODIFIED="1489191093288" TEXT="CmdAccessor"/>
<node CREATED="1489191103187" ID="ID_1016072906" MODIFIED="1489191107214" TEXT="InteractionStateManager"/>
<node CREATED="1489191095700" ID="ID_1053653890" MODIFIED="1489191101056" TEXT="InteractionDirector"/>
</node>
<node CREATED="1489191112026" ID="ID_1805268724" MODIFIED="1489191116245" TEXT="Implementierung">
<node CREATED="1489191118705" ID="ID_1501573546" MODIFIED="1489191120741" TEXT="Aufbau">
<node CREATED="1489191126256" HGAP="24" ID="ID_127004361" MODIFIED="1489777546596" TEXT="Tickets" VSHIFT="26">
<icon BUILTIN="info"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1489191161395" ID="ID_346158251" MODIFIED="1489191839053" TEXT="#1087 GUI/Session command handling">
<linktarget COLOR="#606d94" DESTINATION="ID_346158251" ENDARROW="Default" ENDINCLINATION="842;21;" ID="Arrow_ID_726918944" SOURCE="ID_898481930" STARTARROW="None" STARTINCLINATION="1012;0;"/>
<icon BUILTIN="idea"/>
</node>
<node COLOR="#338800" CREATED="1489191208557" FOLDED="true" ID="ID_632390561" MODIFIED="1490986212354" TEXT="#1088 Command definition registration">
<icon BUILTIN="button_ok"/>
<node CREATED="1490986190364" ID="ID_266121409" MODIFIED="1490986197777" TEXT="CommandSetup helper">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1490986198683" ID="ID_658767230" MODIFIED="1490986204279" TEXT="Build-Skript vertagt">
<icon BUILTIN="hourglass"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1489191229098" ID="ID_341428100" MODIFIED="1489191638320" TEXT="#1089 Command instance management">
<linktarget COLOR="#584d79" DESTINATION="ID_341428100" ENDARROW="Default" ENDINCLINATION="1995;-1317;" ID="Arrow_ID_879741123" SOURCE="ID_1331796226" STARTARROW="None" STARTINCLINATION="334;-13;"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1489196572055" ID="ID_339289633" MODIFIED="1489196577154" TEXT="Command-ID">
<node CREATED="1489196578470" ID="ID_1460242316" MODIFIED="1490976980566" TEXT="extensible ID anschauen">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1490976981548" ID="ID_319912426" MODIFIED="1490977023076" TEXT="nein: es gen&#xfc;gen rein textuelle IDs">
<arrowlink DESTINATION="ID_119293304" ENDARROW="Default" ENDINCLINATION="183;-268;" ID="Arrow_ID_1691148413" STARTARROW="None" STARTINCLINATION="363;0;"/>
</node>
</node>
<node CREATED="1490985775995" ID="ID_1920622498" MODIFIED="1490985782446" TEXT="Instance-Manager">
<node CREATED="1490985786138" ID="ID_664659428" MODIFIED="1490985793244" TEXT="einfache Komponente"/>
<node CREATED="1490985793793" ID="ID_511499018" MODIFIED="1490985798436" TEXT="wo">
<node CREATED="1490985799639" ID="ID_1798979341" MODIFIED="1490985804979" TEXT="SessionCommandService"/>
<node CREATED="1490985805518" ID="ID_937609414" MODIFIED="1490985814593" TEXT="damit vom SessionLoopThread gemanaged"/>
</node>
<node CREATED="1490985820613" ID="ID_1526182884" MODIFIED="1490985823408" TEXT="API">
<node CREATED="1490985825844" ID="ID_1072463021" MODIFIED="1490985853020" TEXT="neue Instanz">
<node CREATED="1490986239341" ID="ID_1121928421" MODIFIED="1490986243065" TEXT="ID dekorieren"/>
<node CREATED="1490986243557" ID="ID_1457927529" MODIFIED="1490986249952" TEXT="slot / smart-ptr erzeugen"/>
</node>
<node CREATED="1490985938597" ID="ID_90790203" MODIFIED="1490985955039" TEXT="Dispatch">
<node CREATED="1490986252963" ID="ID_1745967844" MODIFIED="1490986257991" TEXT="smart-ptr move"/>
<node CREATED="1490986258483" ID="ID_462607969" MODIFIED="1490986261422" TEXT="slot leeren"/>
</node>
</node>
</node>
<node CREATED="1491005426763" ID="ID_1575553505" MODIFIED="1491618872104" TEXT="zu kl&#xe4;ren: duplikate Instanz">
<arrowlink COLOR="#ca978d" DESTINATION="ID_1805144008" ENDARROW="Default" ENDINCLINATION="594;0;" ID="Arrow_ID_191915626" STARTARROW="None" STARTINCLINATION="594;0;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1491005509448" ID="ID_709599195" MODIFIED="1491005512147" TEXT="Fehler?"/>
<node CREATED="1491005514015" ID="ID_66770459" MODIFIED="1491495268579" TEXT="bestehende Instanz &#xfc;berb&#xfc;geln"/>
<node CREATED="1491495244653" ID="ID_1710448495" MODIFIED="1491495254015" TEXT="stillschweigend die bestehende Instanz verwenden"/>
<node CREATED="1491495254699" ID="ID_1848409976" MODIFIED="1491495265858" TEXT="per Fehler abweisen">
<icon BUILTIN="forward"/>
</node>
</node>
<node CREATED="1491008040105" ID="ID_662216926" MODIFIED="1491618848217" TEXT="Implementierung">
<icon BUILTIN="button_ok"/>
<node COLOR="#338800" CREATED="1491008045936" ID="ID_1071153669" MODIFIED="1491008146180" TEXT="CommandInstanceManager_test">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1491008059366" ID="ID_1809405076" MODIFIED="1491008147195" TEXT="CommandInstanceManager">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1491008067046" ID="ID_1905446877" MODIFIED="1491008148076" TEXT="einfache Map-Impl">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1491008073989" ID="ID_1954195404" MODIFIED="1491008148995" TEXT="Symbol-Tabelle">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1491008079676" ID="ID_1262786130" MODIFIED="1491618841600" TEXT="Test">
<icon BUILTIN="button_ok"/>
<node COLOR="#338800" CREATED="1491008088699" ID="ID_1501876595" MODIFIED="1491109806224" TEXT="Basisfall l&#xe4;uft noch nicht">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die Instanz kommt nicht in der Fixture-Queue an
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1491008118695" ID="ID_1595821007" MODIFIED="1491618830886" TEXT="Invocation abdecken">
<icon BUILTIN="button_ok"/>
<node CREATED="1491498978878" ID="ID_1001760576" MODIFIED="1491618830202" TEXT="nicht direkt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      der Umstand, da&#223; Commands auch ausgef&#252;hrt werdern k&#246;nnen,
    </p>
    <p>
      geh&#246;rt nicht zum Thema &quot;Instanz-Management&quot;
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1491499005954" ID="ID_1944400975" MODIFIED="1491618830202" TEXT="aber im Hinblick auf den Lebenszyklus">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn ein Command geht dann in die Queue
    </p>
    <p>
      und kann noch ausgef&#252;hrt werden, w&#228;hrend ein weiteres
    </p>
    <p>
      schon &quot;in der Mache ist&quot;
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node COLOR="#338800" CREATED="1491008124550" ID="ID_249252999" MODIFIED="1491618833169" TEXT="Duplikate abdecken">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1491008129165" ID="ID_118623494" MODIFIED="1491618835441" TEXT="Lebenszyklus-Fehler">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1491753027936" HGAP="35" ID="ID_118278209" MODIFIED="1491757400403" TEXT="Refactoring" VSHIFT="2">
<icon BUILTIN="button_ok"/>
<node CREATED="1491753036895" ID="ID_1029946389" MODIFIED="1491753047201" TEXT="fall-back auf Command integrieren"/>
<node CREATED="1491753645531" ID="ID_597184850" MODIFIED="1491753650607" TEXT="maybeGet entfernen"/>
<node CREATED="1491753047981" ID="ID_1220888974" MODIFIED="1491753330643" TEXT="Referenz rausgeben">
<icon BUILTIN="button_cancel"/>
<node CREATED="1491753058620" ID="ID_1772710050" MODIFIED="1491753078866" TEXT="nicht thradsafe">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1491753170924" ID="ID_1947747818" MODIFIED="1491753316996" TEXT="Command-Zugriff ist by value">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...aus gutem Grund
    </p>
    <p>
      (kann mich erinnern, da&#223; ich mir das &#252;berlegt hatte).
    </p>
    <p>
      Sofern Definitionen wirklich concurrent ge&#228;ndert oder gel&#246;scht werden,
    </p>
    <p>
      k&#246;nnte es sein, da&#223; jemand auf einer stale reference arbeitet,
    </p>
    <p>
      denn das Lock sch&#252;tzt nur den Aufruf innerhalb der CommandRegistry.
    </p>
    <p>
      Sicher ist der Zugriff nur, wenn im Schutzbereich dieses Locks ein
    </p>
    <p>
      neues Command-Objekt kopiert wird. Was allerdings den RefCount erh&#246;ht.
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1491753132377" ID="ID_1441469124" MODIFIED="1491753156451" TEXT="m&#xfc;&#xdf;te daf&#xfc;r direkt auf die CommandRegistry zugreifen">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1491753332318" ID="ID_889636506" MODIFIED="1491757384745" TEXT="nein: Wert rausgeben">
<linktarget COLOR="#3e657d" DESTINATION="ID_889636506" ENDARROW="Default" ENDINCLINATION="21;223;" ID="Arrow_ID_1428975472" SOURCE="ID_1707474386" STARTARROW="None" STARTINCLINATION="-541;0;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1491753337438" ID="ID_1228083262" MODIFIED="1491753342636" TEXT="KISS">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1491753354116" ID="ID_1070880433" MODIFIED="1491753389816" TEXT="Bus + CommandHandler + GenNode">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aber sich mit einem Refcount verr&#252;ckt machen.....
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="smiley-oh"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1491656361886" ID="ID_1269299900" MODIFIED="1491656365558" TEXT="Einbindung">
<icon BUILTIN="flag-yellow"/>
<node COLOR="#338800" CREATED="1491659172438" HGAP="34" ID="ID_1103739498" MODIFIED="1491757376165" TEXT="in SessionCommandService" VSHIFT="-3">
<icon BUILTIN="button_ok"/>
<node CREATED="1491659191155" ID="ID_586991718" MODIFIED="1491659216131" TEXT="Nutzung des Instance-Managers ist optional"/>
<node CREATED="1491659194283" ID="ID_1169808852" MODIFIED="1491659203949" TEXT="direkter Zugriff auf Commands bleibt offen"/>
<node CREATED="1491659224630" ID="ID_837055372" MODIFIED="1491659238608" TEXT="Entscheidung anhand der Command-ID"/>
<node CREATED="1491694300867" ID="ID_1838809343" MODIFIED="1491702954154" TEXT="Festlegung: CommandID wird als Symbol entgegengenommen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....k&#252;nftige Weiterung:
    </p>
    <p>
      auch in EntryID k&#246;nnte ein Symbol-Stecken,
    </p>
    <p>
      mithin in der GenNode::ID
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1491659382161" ID="ID_948768193" MODIFIED="1491702871022" TEXT="Performance: maybeGet">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da dieser Zugriff wirklich f&#252;r jedes Command passiert,
    </p>
    <p>
      m&#246;chte ich mit dem Minimum an Hashtable-Zugriffen auskommen.
    </p>
    <p>
      Daher pr&#252;fen wir als erstes den CommandInstanceManager,
    </p>
    <p>
      da dies der Regelfall ist. Wenn dies scheitert, suchen wir noch
    </p>
    <p>
      in der globalen Registry
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="idea"/>
<icon BUILTIN="stop-sign"/>
<node CREATED="1491702872058" ID="ID_487194706" MODIFIED="1491702875613" TEXT="keine gute Idee"/>
<node CREATED="1491702876137" ID="ID_141674543" MODIFIED="1491702880476" TEXT="doppelt das Interface"/>
<node CREATED="1491702881160" ID="ID_1657270200" MODIFIED="1491753472968" TEXT="Taschenspielerei">
<arrowlink COLOR="#a71f6e" DESTINATION="ID_1776414678" ENDARROW="Default" ENDINCLINATION="-8;-62;" ID="Arrow_ID_559634306" STARTARROW="None" STARTINCLINATION="-86;2;"/>
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node CREATED="1491702805203" HGAP="26" ID="ID_1776414678" MODIFIED="1491757370278" TEXT="Performance / Standard-Fall" VSHIFT="22">
<linktarget COLOR="#a71f6e" DESTINATION="ID_1776414678" ENDARROW="Default" ENDINCLINATION="-8;-62;" ID="Arrow_ID_559634306" SOURCE="ID_1657270200" STARTARROW="None" STARTINCLINATION="-86;2;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1491702822568" ID="ID_1028353876" MODIFIED="1491702830531" TEXT="wir suchen stets zuerst eine Instanz"/>
<node CREATED="1491702831023" ID="ID_184396015" MODIFIED="1491702840810" TEXT="wir fallen auf die globale Registry zur&#xfc;ck"/>
<node CREATED="1491702841453" ID="ID_1707474386" MODIFIED="1491757384744" TEXT="das kann der InstanceManager am Besten selber!">
<arrowlink COLOR="#3e657d" DESTINATION="ID_889636506" ENDARROW="Default" ENDINCLINATION="21;223;" ID="Arrow_ID_1428975472" STARTARROW="None" STARTINCLINATION="-541;0;"/>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1491692398913" HGAP="27" ID="ID_1349137025" MODIFIED="1491753610374" TEXT="in UI-Bus" VSHIFT="6">
<icon BUILTIN="help"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1491692419958" ID="ID_411638678" MODIFIED="1491753600586" TEXT="#1058 consider expanding UI-Bus protocol for command cloning">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1491692431021" ID="ID_1526880552" MODIFIED="1491692436375" TEXT="wollen wir das?"/>
<node CREATED="1491692438907" ID="ID_1137204114" MODIFIED="1491692452509" TEXT="oder nur f&#xfc;r explizite Spezialisierungen?"/>
<node CREATED="1491692779933" ID="ID_184706664" MODIFIED="1491692783680" TEXT="Abw&#xe4;gung">
<node CREATED="1491692784692" ID="ID_875264954" MODIFIED="1491693521708" TEXT="pro">
<icon BUILTIN="button_cancel"/>
<node CREATED="1491692786876" ID="ID_1725123375" MODIFIED="1491692800733" TEXT="Einheitliches Interface"/>
<node CREATED="1491692887517" ID="ID_832151078" MODIFIED="1491692894912" TEXT="Bus k&#xf6;nnte weitere Trennschicht werden"/>
<node CREATED="1491692922625" ID="ID_1378937117" MODIFIED="1491692927444" TEXT="Bus ist gut unit-testbar"/>
</node>
<node CREATED="1491692931040" ID="ID_1678638713" MODIFIED="1491693518988" TEXT="con">
<icon BUILTIN="button_ok"/>
<node CREATED="1491692990840" ID="ID_1200979620" MODIFIED="1491692995939" TEXT="ziemlich indirekt"/>
<node CREATED="1491692996639" ID="ID_1249742943" MODIFIED="1491693005081" TEXT="double-dispatch"/>
<node CREATED="1491693382706" ID="ID_1886288474" MODIFIED="1491693419959" TEXT="CommandHandler hat keinen R&#xfc;ckgabewert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. wir m&#252;&#223;ten dann auch noch das Interface brechen
    </p>
    <p>
      und die Form der ID-Dokoration zur Konvention machen
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1491693197435" ID="ID_1151359036" MODIFIED="1491693375452" TEXT="InteractionState ist kein Tangible">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, f&#252;r das ganze Thema InteractionControl
    </p>
    <p>
      schwebt mir eine Zwischenschicht unabh&#228;ngig von den Widgets vor
    </p>
    <p>
      
    </p>
    <p>
      Wenn nun aber das Anfordern einer neuen Instanz &#252;ber den Bus laufen soll,
    </p>
    <p>
      dann w&#252;rde es wohl ehr direkt von den Tangibles (Widget / Controller) ausgehen.
    </p>
    <p>
      Das wollte ich <i>genau nicht</i>
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1491693246284" ID="ID_553376017" MODIFIED="1491693514745">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Tangible sollte InteractionState <i>verwenden</i>
    </p>
  </body>
</html>
</richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....und demnach sollte InteractionState eben grade nicht von Tangible wissen
    </p>
    <p>
      Demnach m&#252;&#223;te sich InteractionState irgendwo &quot;hinten rum&quot; an den Bus ranmachen,
    </p>
    <p>
      z.B. &#252;ber den InteractionDirector. Das ist aber nun wirklich absurrd,
    </p>
    <p>
      da es letztlich nur darum geht ein <i>ohnehin &#246;ffentliches</i>&#160; Interface aufzurufen
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1491659180516" ID="ID_655217923" MODIFIED="1491659189047" TEXT="in CmdAccess"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1489191252503" ID="ID_1621107057" MODIFIED="1489191308469" TEXT="#1090 Command access for UI-Elements">
<arrowlink COLOR="#758ba4" DESTINATION="ID_827179653" ENDARROW="Default" ENDINCLINATION="173;83;" ID="Arrow_ID_57938013" STARTARROW="None" STARTINCLINATION="550;-93;"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1489196597635" ID="ID_850892898" MODIFIED="1489196604278" TEXT="Access-Front-End">
<node CREATED="1489196605530" ID="ID_1235925214" MODIFIED="1489196611189" TEXT="anschauen">
<node CREATED="1489196613065" ID="ID_1738286518" MODIFIED="1489196617396" TEXT="Proxy-Instanz"/>
<node CREATED="1489196617873" ID="ID_1676140831" MODIFIED="1489196620860" TEXT="Session Services"/>
</node>
</node>
</node>
</node>
<node CREATED="1489777406155" HGAP="74" ID="ID_1319269544" MODIFIED="1489777541271" TEXT="Grundstrukturen" VSHIFT="25">
<node CREATED="1489777414729" ID="ID_464574092" MODIFIED="1489777420276" TEXT="CommandID"/>
<node CREATED="1489777420817" ID="ID_1146990209" MODIFIED="1489777427435" TEXT="Command access"/>
<node CREATED="1489777429111" ID="ID_148471534" MODIFIED="1489777436010" TEXT="InvocationTrail"/>
<node CREATED="1489777438663" ID="ID_729917092" MODIFIED="1489777452519" TEXT="InteractionState"/>
<node CREATED="1489777502366" ID="ID_1798143241" MODIFIED="1489777514432" TEXT="Command-Definitionen"/>
<node CREATED="1489777517315" ID="ID_922436346" MODIFIED="1489777522606" TEXT="instance management"/>
<node CREATED="1489777523266" ID="ID_136474944" MODIFIED="1489777532037" TEXT="trigger Invocation"/>
</node>
<node CREATED="1489778159357" HGAP="105" ID="ID_1046149430" MODIFIED="1489778168498" TEXT="Command-Zyklus" VSHIFT="36">
<node CREATED="1489544225984" ID="ID_695057205" MODIFIED="1489544240309" TEXT="Triggern">
<icon BUILTIN="forward"/>
<node CREATED="1489544261547" ID="ID_782648362" MODIFIED="1489544266933" TEXT="wer">
<icon BUILTIN="help"/>
<node CREATED="1489544403984" ID="ID_382753424" MODIFIED="1489544416135" TEXT="&quot;ein UI-Element&quot;">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1489544439187" ID="ID_278628492" MODIFIED="1489544450590" TEXT="Beispiel: Toolbar-Button"/>
<node CREATED="1489544452034" ID="ID_1252070479" MODIFIED="1489544459404" TEXT="Beispiel: Kontextmen&#xfc;-Eintrag"/>
<node CREATED="1489544513577" ID="ID_1918576059" MODIFIED="1489544521044" TEXT="Beispiel: Aktion per Keybinding"/>
<node CREATED="1489544524128" ID="ID_620530211" MODIFIED="1489544534314" TEXT="Beispiel: Signal drag-n-drop"/>
</node>
<node CREATED="1489544268018" ID="ID_317151852" MODIFIED="1489544273089" TEXT="was">
<icon BUILTIN="help"/>
<node CREATED="1489544652414" ID="ID_61156502" MODIFIED="1489544658199" TEXT="lokal bekannt"/>
<node CREATED="1489544640288" ID="ID_1660798945" MODIFIED="1489544651898" TEXT="fest verdrahtet"/>
<node CREATED="1489544659597" ID="ID_519858607" MODIFIED="1489544666376" TEXT="ggfs. Auswahl aus kleiner Menge"/>
</node>
<node CREATED="1489544717333" ID="ID_154924026" MODIFIED="1489544720305" TEXT="falls">
<icon BUILTIN="help"/>
<node CREATED="1489544722261" ID="ID_1252784107" MODIFIED="1489544725320" TEXT="ausf&#xfc;hrbar"/>
<node CREATED="1489544725860" ID="ID_1860103561" MODIFIED="1489544728848" TEXT="d.h. aktiviert"/>
<node CREATED="1489544826383" ID="ID_1190793385" MODIFIED="1489544970504" TEXT="zwei verschiedene Effekte">
<icon BUILTIN="idea"/>
<node CREATED="1489544836757" ID="ID_1364306635" MODIFIED="1489544891968" TEXT="Ausf&#xfc;hrung abweisen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...stattdessen einen Fehler-Indikator ausl&#246;sen
    </p>
    <p>
      (Beispiel &quot;in-point fehlt&quot;)
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1489544910395" ID="ID_909938815" MODIFIED="1489544965386" TEXT="enable Action">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das ist eine Reaktion,
    </p>
    <p>
      die von einem managing Ui-Element ausgef&#252;hrt wird,
    </p>
    <p>
      aber von einem externen State-Change getriggert wird
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1491005020528" ID="ID_1665806173" MODIFIED="1491005024747" TEXT="zu kl&#xe4;ren">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1491005025928" ID="ID_1805144008" MODIFIED="1491005470990" STYLE="fork" TEXT="duplikate Instanzen">
<linktarget COLOR="#ca978d" DESTINATION="ID_1805144008" ENDARROW="Default" ENDINCLINATION="594;0;" ID="Arrow_ID_191915626" SOURCE="ID_1575553505" STARTARROW="None" STARTINCLINATION="594;0;"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1491005039414" ID="ID_378645276" MODIFIED="1491005470990" TEXT="mehrfach newInstance() mit gleicher ID"/>
<node CREATED="1491005054595" ID="ID_1987692266" MODIFIED="1491495289915" TEXT="drei M&#xf6;glichkeiten">
<icon BUILTIN="info"/>
<node CREATED="1491005058931" ID="ID_1108733153" MODIFIED="1491005470990" TEXT="mit neuem Klon &#xfc;berschreiben"/>
<node CREATED="1491495291966" ID="ID_703254591" MODIFIED="1491495302281" TEXT="stillschweigend bestehende Instanz ausgeben"/>
<node CREATED="1491005066706" ID="ID_1990482123" MODIFIED="1491495433236" TEXT="Fehler ausl&#xf6;sen">
<icon BUILTIN="forward"/>
<node CREATED="1491495356533" ID="ID_1948190107" MODIFIED="1491495397512" TEXT="einzig sichere Variante">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1491495398895" ID="ID_1958544932" MODIFIED="1491495404395" TEXT="es gibt jetzt auch getInstance"/>
<node CREATED="1491495405151" ID="ID_988620199" MODIFIED="1491495425552" TEXT="der Client mu&#xdf; geht von Exclusivit&#xe4;t aus"/>
</node>
</node>
<node CREATED="1491005071634" ID="ID_178338289" MODIFIED="1491005470990" TEXT="zur Kl&#xe4;rung">
<icon BUILTIN="bell"/>
<node CREATED="1491005078409" ID="ID_1515823316" MODIFIED="1491005470990" TEXT="...mu&#xdf; ich wissen, wie newInstance verwendet wird"/>
<node CREATED="1491005491226" ID="ID_1986906306" MODIFIED="1491005499069" TEXT="und das h&#xe4;ngt von InvocationTrail ab"/>
</node>
</node>
<node CREATED="1491494748185" ID="ID_1589354901" MODIFIED="1491494763342" TEXT="Instanzen registrieren">
<icon BUILTIN="help"/>
<node CREATED="1491494779796" ID="ID_1437580350" MODIFIED="1491494872286" TEXT="oder nur Prototypen?">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, es geht um die Haupt-Registry f&#252;r Commands.
    </p>
    <p>
      Wenn wir eine Instanz machen, um Parameter zu binden und sie dann schlie&#223;lich auszuf&#252;hren,
    </p>
    <p>
      k&#246;nnte man dieser Instanz einen Namen geben, und sie in die Haupt-Registry eintragen..
    </p>
    <p>
      Oder man k&#246;nnte sie anonym verarbeiten, weil Command selber ein smart-Handle ist.
    </p>
  </body>
</html>
</richcontent>
</node>
<node CREATED="1491494889781" ID="ID_737410509" MODIFIED="1491495069450" TEXT="Analyse">
<icon BUILTIN="idea"/>
<node CREATED="1491494903779" ID="ID_1388732102" MODIFIED="1491495065587" TEXT="ja">
<icon BUILTIN="button_cancel"/>
<node CREATED="1491494912266" ID="ID_32553701" MODIFIED="1491494926628" TEXT="Vorteil: Zugriff per-Name"/>
<node CREATED="1491494928040" ID="ID_76613856" MODIFIED="1491494938066" TEXT="Nachteil: Druck auf die Registry"/>
<node CREATED="1491494938614" ID="ID_189174601" MODIFIED="1491494953016" TEXT="Nachteil: IDs m&#xfc;ssen verwaltet werden"/>
</node>
<node CREATED="1491494957020" ID="ID_266987025" MODIFIED="1491495063214" TEXT="nein">
<icon BUILTIN="button_ok"/>
<node CREATED="1491494972658" ID="ID_1987998531" MODIFIED="1491494987067" TEXT="Vorteil: spart jede Menge Infrastruktur"/>
<node CREATED="1491494989215" ID="ID_1302849116" MODIFIED="1491495018271" TEXT="Nachteil: Aufruf-Protokol sehr festgelegt"/>
</node>
</node>
<node CREATED="1491495071460" ID="ID_597368143" MODIFIED="1491495081513" TEXT="wir registrieren nicht">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node COLOR="#252298" CREATED="1489546623162" HGAP="-24" ID="ID_1622574347" MODIFIED="1489778039083" TEXT="was wird gebraucht" VSHIFT="21">
<linktarget COLOR="#352c7d" DESTINATION="ID_1622574347" ENDARROW="Default" ENDINCLINATION="39;-3322;" ID="Arrow_ID_1512127407" SOURCE="ID_1401258681" STARTARROW="None" STARTINCLINATION="2000;0;"/>
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="help"/>
<node COLOR="#ff0000" CREATED="1489546678250" ID="ID_228157787" MODIFIED="1489547976088" TEXT="Vorsicht UI-Programmierung">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1489546705798" ID="ID_43582318" MODIFIED="1489546746697" TEXT="keine neue Programmiersprache entwerfen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1489546724892" ID="ID_1763781107" MODIFIED="1489546749259" TEXT="mu&#xdf; letztlich explizit sein">
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1489546837172" ID="ID_665817500" MODIFIED="1489546841173" TEXT="aber">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1489546843356" ID="ID_1556795494" MODIFIED="1489546854590" TEXT="ich will Gestaltbarkeit"/>
<node CREATED="1489546855202" ID="ID_1824748232" MODIFIED="1489546862101" TEXT="ich will Konfigurierbarkeit"/>
</node>
<node CREATED="1489546994743" ID="ID_1851585461" MODIFIED="1489546998138" TEXT="im Einzelnen">
<node CREATED="1489777326111" ID="ID_497480483" MODIFIED="1489777329632" TEXT="CommandID">
<node CREATED="1489715248861" ID="ID_999876408" MODIFIED="1489715252392" TEXT="brauche">
<node CREATED="1489715276057" ID="ID_878658276" MODIFIED="1489715279093" TEXT="Basis-ID"/>
<node CREATED="1489715297406" ID="ID_601143379" MODIFIED="1489715305201" TEXT="dekoriert durch Invocation-Trail"/>
<node CREATED="1489715320435" ID="ID_1521007439" MODIFIED="1491003112168" TEXT="dekoriert durch laufende Nr">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1489715326210" ID="ID_1820734132" MODIFIED="1489715331310" TEXT="guten Hash von diesen"/>
<node CREATED="1489715461992" ID="ID_115322523" MODIFIED="1491003142993" TEXT="&quot;leere&quot; Basis-ID ohne lfdNr">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1489715591582" ID="ID_256884690" MODIFIED="1491003145193" TEXT="inkrement der lfdNr">
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node CREATED="1489716825126" HGAP="38" ID="ID_969368347" MODIFIED="1489717783561" TEXT="Ans&#xe4;tze" VSHIFT="-1">
<node CREATED="1489716845980" ID="ID_816994670" MODIFIED="1489718875217" TEXT="Command-ID-Token">
<icon BUILTIN="button_cancel"/>
<node CREATED="1489716859034" ID="ID_1342799459" MODIFIED="1489716867940" TEXT="SubID o.&#xe4;."/>
<node CREATED="1489716870000" ID="ID_907818022" MODIFIED="1489716875547" TEXT="Wert-Semantik"/>
<node CREATED="1489716876183" ID="ID_880911498" MODIFIED="1489716883650" TEXT="enth&#xe4;lt die lfdNr"/>
</node>
<node CREATED="1489717087626" ID="ID_1304379039" MODIFIED="1489718872841" TEXT="Registrierungs-Instanz">
<icon BUILTIN="button_cancel"/>
<node CREATED="1489717094881" ID="ID_890501908" MODIFIED="1489717104332" TEXT="Referenz-Semantik"/>
<node CREATED="1489717108735" ID="ID_142693821" MODIFIED="1489717115730" TEXT="enth&#xe4;lt direkt den Z&#xe4;hler"/>
<node CREATED="1489717116246" ID="ID_1967376157" MODIFIED="1489717129232" TEXT="brauche front-End-Token"/>
</node>
<node CREATED="1489717203882" ID="ID_199366549" MODIFIED="1489718870140" TEXT="eingebettete Information">
<icon BUILTIN="button_ok"/>
<node CREATED="1489717237622" ID="ID_1384169115" MODIFIED="1489717301257" TEXT="Command + Z&#xe4;hler in Hashtable"/>
<node CREATED="1489717304621" ID="ID_1984888825" MODIFIED="1489717311967" TEXT="CmdInstanceManager hat Hashtable"/>
<node CREATED="1489717425684" ID="ID_205251145" MODIFIED="1489717436846" TEXT="Instance-Management komplett eingekapselt"/>
<node CREATED="1489717312532" ID="ID_1911293548" MODIFIED="1489717329029" TEXT="key ist Basis-Cmd-ID + InvocationTrail-ID"/>
<node CREATED="1489717395312" ID="ID_1098297494" MODIFIED="1489717405571" TEXT="InvocationTrail speichert nur diese ID"/>
<node CREATED="1489717406447" ID="ID_1938762085" MODIFIED="1489717411690" TEXT="Bang sendet nur diese ID"/>
</node>
</node>
<node CREATED="1489717768805" HGAP="79" ID="ID_119293304" MODIFIED="1490977023076" TEXT="Entscheidung" VSHIFT="4">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_119293304" ENDARROW="Default" ENDINCLINATION="183;-268;" ID="Arrow_ID_1691148413" SOURCE="ID_319912426" STARTARROW="None" STARTINCLINATION="363;0;"/>
<icon BUILTIN="yes"/>
<node CREATED="1489717797353" ID="ID_899130819" MODIFIED="1489717807068" TEXT="die ersten beiden sind un&#xf6;tig generisch"/>
<node CREATED="1489717807872" ID="ID_1209104496" MODIFIED="1489717818826" TEXT="wir bauen ohnehin den Command-Cycle fest ein"/>
<node CREATED="1489717844003" ID="ID_97055123" MODIFIED="1489717853605" TEXT="letztgenannte L&#xf6;sung ist auf den Punkt"/>
<node CREATED="1489717927279" ID="ID_845301501" MODIFIED="1489717959263" TEXT="Command-ID erf&#xfc;llt keine regulierende Funktion"/>
<node CREATED="1491003369611" ID="ID_854080320" MODIFIED="1491003437055" TEXT="Instanz-Management funktioniert anonym">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...weil es zu jedem InvocationPath
    </p>
    <p>
      zu jeder Zeit nur eine &quot;offene&quot; Instanz gibt.
    </p>
    <p>
      Also gen&#252;gt es, einen anonymen Klon dieser Instanz zu halten
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1489717960331" ID="ID_1849715968" MODIFIED="1489717991603" TEXT="viel wichtiger sind die Aktivierungs-Regeln"/>
</node>
</node>
<node CREATED="1489546998918" HGAP="43" ID="ID_668687712" MODIFIED="1491495585626" TEXT="Aktivierung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint ist:
    </p>
    <p>
      Ein UI-Control wird <i>aktivierbar,</i>&#160;weil das zugrundeliegende Command
    </p>
    <p>
      alle seine Argumente aus dem aktuellen Kontext befriedigen kann
    </p>
    <p>
      
    </p>
    <p>
      Beispiel: Men&#252;-Eintrag &quot;create duplicate&quot;
    </p>
  </body>
</html>
</richcontent>
<node CREATED="1489547002918" ID="ID_728456926" MODIFIED="1489547015360" TEXT="Callback installieren"/>
<node CREATED="1489547016564" ID="ID_41325273" MODIFIED="1489547028606" TEXT="Anfrage: ist dies aktivierbar?"/>
</node>
<node CREATED="1489547086458" HGAP="48" ID="ID_1858100202" MODIFIED="1489781264701" TEXT="Argumente" VSHIFT="-1">
<node CREATED="1489547096857" ID="ID_184341736" MODIFIED="1489547103300" TEXT="&quot;gib mir die Argumente!&quot;">
<node CREATED="1489547170463" ID="ID_1981926549" MODIFIED="1489547178266" TEXT="wirklich?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1489547179254" ID="ID_192306839" MODIFIED="1489547197378" TEXT="ist das der richtige Ansatz?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1489547208282" ID="ID_99440101" MODIFIED="1489690731089" TEXT="l&#xe4;&#xdf;t sich das &#xfc;berhaupt je realisieren?">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1489547112255" ID="ID_270498008" MODIFIED="1489548210093" TEXT="generische Rollen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die Idee ist hier,
    </p>
    <p>
      da&#223; diese generischen Rollen bereits in der Einrichtung der Command-Definition verwendet werden.
    </p>
    <p>
      Das hei&#223;t, f&#252;r einen bestimmten Invocation-Trail legt man fest,
    </p>
    <p>
      da&#223; ein bestimmtes Argument an eine gewisse Rolle gebunden wird,
    </p>
    <p>
      oder andernfalls einen bestimmten Namen bekommt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1489547124597" ID="ID_408090908" MODIFIED="1489547128224" TEXT="das Subjekt"/>
<node CREATED="1489547128837" ID="ID_865724442" MODIFIED="1489547131120" TEXT="der Container"/>
<node CREATED="1489547498122" ID="ID_1669169317" MODIFIED="1489547504003" TEXT="die Dauer"/>
<node CREATED="1489547283807" ID="ID_559442696" MODIFIED="1489547286467" TEXT="der Wert"/>
</node>
<node CREATED="1489547506625" ID="ID_1063184971" MODIFIED="1489719071018" TEXT="konkret....">
<arrowlink COLOR="#4d2a67" DESTINATION="ID_1622068086" ENDARROW="Default" ENDINCLINATION="-9;-36;" ID="Arrow_ID_717312728" STARTARROW="None" STARTINCLINATION="15;49;"/>
<node CREATED="1489547511224" ID="ID_954080965" MODIFIED="1489547530586" TEXT="das UI-Element wei&#xdf; explizit was gebraucht wird"/>
<node CREATED="1489547542852" ID="ID_271774122" MODIFIED="1489547548943" TEXT="und stellt selber das Tupel zusammen"/>
<node CREATED="1489547574504" ID="ID_49978062" MODIFIED="1489547582019" TEXT="oder ruft einen Funktor auf"/>
<node CREATED="1489547588102" ID="ID_985840704" MODIFIED="1489547597465" TEXT="wodurch die Argument-Typen dokumentiert sind"/>
</node>
</node>
<node CREATED="1489548252324" HGAP="40" ID="ID_1622068086" MODIFIED="1489781254726" TEXT="das k&#xf6;nnte der InvocationTrail sein" VSHIFT="26">
<linktarget COLOR="#4d2a67" DESTINATION="ID_1622068086" ENDARROW="Default" ENDINCLINATION="-9;-36;" ID="Arrow_ID_717312728" SOURCE="ID_1063184971" STARTARROW="None" STARTINCLINATION="15;49;"/>
<icon BUILTIN="idea"/>
<node CREATED="1489719178053" ID="ID_1509284806" MODIFIED="1489719184696" TEXT="hat Cmd-ID + eigene ID"/>
<node CREATED="1489719186276" ID="ID_929080602" MODIFIED="1489719196230" TEXT="kann entscheiden ob ausf&#xfc;hrbar"/>
<node CREATED="1489719200330" ID="ID_840062807" MODIFIED="1489719208197" TEXT="hat Argument-Accessor"/>
<node COLOR="#5e427f" CREATED="1489719212976" HGAP="-52" ID="ID_949699860" MODIFIED="1489719253576" TEXT="noch offen" VSHIFT="45">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1489719257426" ID="ID_1618753270" MODIFIED="1489719266032" TEXT="wer erzeugt den InvocationTrail"/>
<node CREATED="1489719267537" ID="ID_963145148" MODIFIED="1489719291065" TEXT="was ist bereits vom Command her definiert"/>
<node CREATED="1489719292181" ID="ID_804928267" MODIFIED="1489719304359" TEXT="kann es mehrere konkurrierende Auslegungen geben?"/>
<node CREATED="1489719311347" ID="ID_1220954776" MODIFIED="1489719322437" TEXT="oder ist eine Zugriffsstrategie = ein InvocationTrail"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1489781392694" ID="ID_1312397991" MODIFIED="1489781396114" TEXT="Festlegungen">
<node CREATED="1489781404925" ID="ID_1918933171" MODIFIED="1489781408881" TEXT="Command-ID">
<node CREATED="1489781409876" ID="ID_1973020417" MODIFIED="1491003215267" TEXT="Basis-ID ist ein Symbol"/>
<node CREATED="1491003215783" ID="ID_1537560721" MODIFIED="1491003227609" TEXT="es wird mit einer Invocation-ID dekoriert"/>
<node CREATED="1491003228101" ID="ID_661472357" MODIFIED="1491003254161" TEXT="wir brauchen keine laufende Nummer">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1491003237108" ID="ID_1167308664" MODIFIED="1491003252150" TEXT="Instance-Management funktioniert anonym"/>
</node>
<node CREATED="1489781426978" ID="ID_1543445405" MODIFIED="1490985671060" TEXT="konkrete Definition">
<icon BUILTIN="button_ok"/>
<node CREATED="1489781443184" ID="ID_966382785" MODIFIED="1489781445387" TEXT="Ort">
<node CREATED="1489781447639" ID="ID_588776773" MODIFIED="1489781474176" TEXT="proc/cmd/*-cmd.cpp"/>
<node CREATED="1489781476355" ID="ID_1953562265" MODIFIED="1489781480998" TEXT="Namespace proc::cmd"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1489781611257" FOLDED="true" ID="ID_179887792" MODIFIED="1490985665139" TEXT="brauche">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1489781616257" ID="ID_1645536558" MODIFIED="1489781625283" TEXT="Schema f&#xfc;r ID-Konstanten"/>
<node CREATED="1489781628743" ID="ID_1733247268" MODIFIED="1489781635194" TEXT="Mechanismus, der die Definition treibt"/>
<node CREATED="1489781758389" ID="ID_1124650374" MODIFIED="1489781765521" TEXT="gew&#xfc;nscht">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1489781775443" ID="ID_379002135" MODIFIED="1489785133742" TEXT="Name und Definition nahe zusammen"/>
<node CREATED="1489781769500" ID="ID_998243238" MODIFIED="1489781774456" TEXT="keine Redundanz"/>
<node CREATED="1489781902770" ID="ID_1568226956" MODIFIED="1489781907366" TEXT="gut lesbar"/>
<node CREATED="1489781907906" ID="ID_1206063892" MODIFIED="1489781915492" TEXT="debuggbar"/>
<node CREATED="1489781923640" ID="ID_241861600" MODIFIED="1489781930043" TEXT="kommentierbar (Doxygen)"/>
<node CREATED="1489785114574" ID="ID_241665479" MODIFIED="1489785123487" TEXT="nur unsichtbare Magie"/>
</node>
<node CREATED="1489785137083" FOLDED="true" ID="ID_1466955376" MODIFIED="1490927752773" TEXT="M&#xf6;glichkeiten">
<node CREATED="1489785163216" ID="ID_1992894903" MODIFIED="1489785168547" TEXT="zu Fu&#xdf;...">
<node CREATED="1489785199131" ID="ID_111396262" MODIFIED="1489785207662" TEXT="ID-Konstanten im Header deklarieren"/>
<node CREATED="1489785208298" ID="ID_315886293" MODIFIED="1489785222188" TEXT="ID-Konstanten in der jeweiligen translation-Unit definieren"/>
<node CREATED="1489785250500" ID="ID_464247309" MODIFIED="1489785259671" TEXT="Lifecycle-Hook installieren..."/>
<node CREATED="1489785260523" ID="ID_1306260885" MODIFIED="1489785270590" TEXT="..der eine Registrierungs-Funktion aufruft"/>
<node CREATED="1489785276137" ID="ID_1526475921" MODIFIED="1489785285307" TEXT="in dieser die CommandDef absetzen"/>
<node CREATED="1489785285872" ID="ID_1304462599" MODIFIED="1489785296938" TEXT="und als Command-ID die definierte Konstante verwenden"/>
</node>
<node CREATED="1489786040203" ID="ID_1417335273" MODIFIED="1489786045870" TEXT="Vereinfachungen">
<node CREATED="1489786055057" ID="ID_565306146" MODIFIED="1489786059852" TEXT="decl/def">
<node CREATED="1489786238273" ID="ID_1957761383" MODIFIED="1489786331600" TEXT="marker-Makro"/>
<node CREATED="1489786245024" ID="ID_430222749" MODIFIED="1489786255810" TEXT="alle Definitionen in Makros einwickeln"/>
<node CREATED="1489786393908" ID="ID_1345826013" MODIFIED="1489786660959" TEXT="spezielle Inclusion-Rituale">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vermutlich l&#228;uft es immer darauf hinaus
    </p>
    <ul>
      <li>
        da&#223; cmd.hpp die Implementierungs-Einheiten includiert
      </li>
      <li>
        oder da&#223; in einer ausgezeichneten Impl-Einheit das marker-Makro gesetzt wird und dann cmd.hpp includiert wird
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1489789880260" ID="ID_768121026" MODIFIED="1489789887822" TEXT="Template-Spezialisierung?">
<node CREATED="1489790030727" ID="ID_1780788892" MODIFIED="1489790046961" TEXT="k&#xf6;nnte const char* nehmen"/>
<node CREATED="1489790053044" ID="ID_1594786428" MODIFIED="1489790070534" TEXT="k&#xf6;nnte dann nach Literal konvertibel sein"/>
<node CREATED="1489790158335" ID="ID_1828195626" MODIFIED="1489790166017" TEXT="Template k&#xf6;nnte in Header definiert sein"/>
<node CREATED="1489790167669" ID="ID_156312862" MODIFIED="1489790182247" TEXT="und nur die explizite Spezialisierung generiert Code"/>
</node>
<node CREATED="1489790425251" ID="ID_1099709668" MODIFIED="1489790430606" TEXT="auto-Registrierung">
<node CREATED="1489790431810" ID="ID_1253214974" MODIFIED="1489790442380" TEXT="variable statisch initialisieren"/>
<node CREATED="1489790522958" ID="ID_1454213723" MODIFIED="1489790532345" TEXT="Lambda an Basis-Ctor geben"/>
</node>
<node CREATED="1489790555393" ID="ID_1281815871" MODIFIED="1489790574611" TEXT="DSL wie boost-test">
<node CREATED="1489790576831" ID="ID_84712572" MODIFIED="1489790584658" TEXT="definiert Klasse o.&#xe4;"/>
<node CREATED="1489790588749" ID="ID_1302974476" MODIFIED="1489790599656" TEXT="User schreibt direkt dahinter einen Codeblock"/>
<node CREATED="1489790689536" ID="ID_605065689" MODIFIED="1489790702050" TEXT="tats&#xe4;chlich wird das eine out-of-class definierte Methode"/>
<node CREATED="1489791695217" ID="ID_1529943414" MODIFIED="1489791705036" TEXT="...und kann sogar per Doxygen kommentiert werden"/>
</node>
<node CREATED="1489791741539" ID="ID_175521756" MODIFIED="1489791746686" TEXT="im Build-Proze&#xdf; parsen">
<node CREATED="1489791753074" ID="ID_1141751955" MODIFIED="1489791761893" TEXT="und z.B. den Header daraus generieren"/>
<node CREATED="1489791763313" ID="ID_1622970394" MODIFIED="1489791766588" TEXT="warum nicht...?"/>
<node CREATED="1489791777102" ID="ID_102161218" MODIFIED="1489791786937" TEXT="empfinde ich besser als komplex verschachtelte Makros"/>
</node>
</node>
</node>
<node CREATED="1489791729957" ID="ID_1986771449" MODIFIED="1490927727826" TEXT="kombinierte Magie">
<icon BUILTIN="button_ok"/>
<node CREATED="1489791798116" ID="ID_197500257" MODIFIED="1490927767187" TEXT="Definitions-Klasse">
<icon BUILTIN="idea"/>
<node CREATED="1489791805675" ID="ID_1517636242" MODIFIED="1489811237752" TEXT="ist konvertierbar nach Literal">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1489791817873" ID="ID_361574240" MODIFIED="1489811240833" TEXT="nimmt Lambda als Konstruktor-Argument">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1489791849245" ID="ID_262482724" MODIFIED="1489791860879" TEXT="bietet interne DSL"/>
<node CREATED="1489795277835" ID="ID_663997649" MODIFIED="1489795286417" TEXT="Name gesucht...?">
<icon BUILTIN="help"/>
<node CREATED="1489795288354" ID="ID_960791386" MODIFIED="1489795346577" TEXT="CommandInstance">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1489795292689" ID="ID_1059834523" MODIFIED="1489795346578" TEXT="CommandDefinition">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1489795318614" ID="ID_437740295" MODIFIED="1489795346578" TEXT="CommandName">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1489795321669" ID="ID_115829263" MODIFIED="1489795346579" TEXT="CommandItem">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1489795298832" ID="ID_1272194079" MODIFIED="1489795342763" TEXT="CommandSetup">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1489791864787" ID="ID_891631217" MODIFIED="1489791877373" TEXT="in den Impl-Einheiten eine Folge von Definitionen"/>
<node CREATED="1489791877913" ID="ID_387208356" MODIFIED="1489793604385" TEXT="zugh&#xf6;rigen Header proc/cmd.hpp im Build-Proze&#xdf; regenerieren">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1489811274588" ID="ID_1635051629" MODIFIED="1489811278807" TEXT="Mechanismus">
<node COLOR="#338800" CREATED="1489811285746" ID="ID_1726037509" MODIFIED="1490927663610" TEXT="CommandSetup">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1489811291313" ID="ID_1780064476" MODIFIED="1490927667601" TEXT="Makro COMMAND_DEFINITION">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1489811304111" ID="ID_1976451454" MODIFIED="1490927673243" TEXT="Build-Generierung">
<icon BUILTIN="hourglass"/>
<node CREATED="1490927680004" ID="ID_1456638216" MODIFIED="1490927687183" TEXT="auf &quot;sp&#xe4;ter&quot; vertagt"/>
<node CREATED="1490927702128" ID="ID_1355040899" MODIFIED="1490927711041" TEXT="wenn die h&#xe4;ndische Pflege zum Problem wird"/>
</node>
</node>
</node>
<node CREATED="1490985673313" ID="ID_1705714293" MODIFIED="1490985677228" TEXT="Instanzen">
<node CREATED="1490985683359" ID="ID_476048128" MODIFIED="1490985691466" TEXT="Instanz-Manager">
<node CREATED="1490985692550" ID="ID_300485716" MODIFIED="1490985696321" TEXT="einfache Komponente"/>
<node CREATED="1490985710963" ID="ID_447657694" MODIFIED="1490985717974" TEXT="keinerlei Lifecycle-Magie"/>
<node CREATED="1490985720722" ID="ID_1386937508" MODIFIED="1490985736292" TEXT="ist nur eine Registry mit etwas Funktionalit&#xe4;t">
<node CREATED="1490986294710" ID="ID_558020871" MODIFIED="1490986301601" TEXT="Comand-IDs dekorieren"/>
<node CREATED="1490986302197" ID="ID_1305641719" MODIFIED="1491004967731" TEXT="fortlaufenden Z&#xe4;hler verwalten">
<icon BUILTIN="button_cancel"/>
<node CREATED="1490986345879" ID="ID_1736956933" MODIFIED="1490986393723" TEXT="denn: Dispatch bedeutet Verz&#xf6;gerung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...also ist eine Instanz durchaus noch am Leben,
    </p>
    <p>
      w&#228;hrend bereits die n&#228;chste Instanz f&#252;r das GUI ausgegeben wurde.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1490986395088" ID="ID_52247198" MODIFIED="1490986412151" TEXT="Stets nur eine Instanz im Instance-manager">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1490986336425" ID="ID_1955343463" MODIFIED="1490986430233" TEXT="Konsequenz: leerer Eintrag">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...damit die Nummer erhalten bleibt
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1491004971039" ID="ID_329531567" MODIFIED="1491004978482" TEXT="anonyme Instanz verwalten">
<node CREATED="1491004982341" ID="ID_1956457610" MODIFIED="1491495639478" TEXT="Frage: Duplikat &#xfc;berschreiben?">
<icon BUILTIN="help"/>
<node CREATED="1491495641174" ID="ID_1196432780" MODIFIED="1491495651041" TEXT="antwort (vorerst): nein"/>
<node CREATED="1491495651932" ID="ID_1393507104" MODIFIED="1491495656080" TEXT="w&#xe4;re gef&#xe4;hrlich"/>
<node CREATED="1491495656604" ID="ID_1592570029" MODIFIED="1491495669470" TEXT="weist auf Logik-Fehler hin"/>
</node>
<node CREATED="1491005133794" ID="ID_1003096513" MODIFIED="1491005393601" TEXT="vorl&#xe4;ufig &#xfc;berschreibe ich stillschweigend">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1490986309084" ID="ID_1432845924" MODIFIED="1490986319583" TEXT="sicher &#xdc;bergabe an Dispatch">
<node CREATED="1490986320434" ID="ID_1807848022" MODIFIED="1490986324558" TEXT="Dispatch als Closure"/>
<node CREATED="1490986325626" ID="ID_167499115" MODIFIED="1490986333421" TEXT="Exception-Safety!"/>
</node>
</node>
<node CREATED="1491003278638" ID="ID_516679033" MODIFIED="1491003300696" TEXT="Instanz-Management">
<node CREATED="1491003301379" ID="ID_997022970" MODIFIED="1491003305671" TEXT="funktioniert fast von selbst"/>
<node CREATED="1491003306451" ID="ID_1812101347" MODIFIED="1491003314278" TEXT="Command == smart-Handle"/>
<node CREATED="1491003314746" ID="ID_850841535" MODIFIED="1491003350337" TEXT="instanz kann anonym bleiben">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1491003323585" ID="ID_719524765" MODIFIED="1491003347758" TEXT="die lokale Registry h&#xe4;llt sie am Leben">
<icon BUILTIN="info"/>
</node>
</node>
<node CREATED="1490985696821" ID="ID_564026288" MODIFIED="1490985706464" TEXT="wird vom SessionCommandService betrieben"/>
</node>
<node CREATED="1490985748207" ID="ID_1314513556" MODIFIED="1490985751457" TEXT="Instanzbildung">
<node CREATED="1490985756381" ID="ID_1346029720" MODIFIED="1490985767448" TEXT="wird durch ID-Dekoration getrieben"/>
<node CREATED="1491656301121" ID="ID_566719122" MODIFIED="1491656320281" TEXT="es ist stets nur eine Instanz offen"/>
<node CREATED="1491656321252" ID="ID_1903067692" MODIFIED="1491656336277" TEXT="Widerspr&#xfc;che sind Hiweis auf Fehlfunktion"/>
<node CREATED="1491694174845" ID="ID_344801148" MODIFIED="1491694242610" TEXT="Instanzbildung geh&#xf6;rt nicht in das Tangible">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das ist ein grundlegender Beschlu&#223;.
    </p>
    <p>
      InteractionControl ist eine eigene Schicht;
    </p>
    <p>
      deshalb ist auch der UI-Bus nicht <i>das </i>Universal-Interface schlechthin
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1488672593812" ID="ID_1454459609" MODIFIED="1488672598271" TEXT="konkret"/>
</node>
<node CREATED="1448063874479" HGAP="43" ID="ID_739054690" MODIFIED="1481413149364" TEXT="UI-Modell" VSHIFT="1">
<icon BUILTIN="help"/>
</node>
<node CREATED="1434128074725" HGAP="28" ID="ID_933994138" MODIFIED="1488423307214" TEXT="Diff-System" VSHIFT="1">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="go"/>
<node CREATED="1434128278990" ID="ID_106354755" MODIFIED="1434128283641" TEXT="Diff-Darstellung"/>
<node CREATED="1434128267381" ID="ID_823706141" MODIFIED="1434128551925" TEXT="List-diff">
<icon BUILTIN="go"/>
</node>
<node CREATED="1434128078638" ID="ID_1704749549" MODIFIED="1473353206035" TEXT="Tree-Diff">
<icon BUILTIN="go"/>
<node CREATED="1434128095838" ID="ID_419405890" MODIFIED="1434128561967" TEXT="Detector">
<icon BUILTIN="stop"/>
</node>
<node CREATED="1434128092877" ID="ID_105246595" MODIFIED="1434128109517" TEXT="Applikator">
<node CREATED="1434128115462" ID="ID_1299653797" MODIFIED="1434128119065" TEXT="Tree-Mutator"/>
</node>
<node CREATED="1434128083878" ID="ID_937754899" MODIFIED="1434128494612" TEXT="Format">
<node CREATED="1434128153773" FOLDED="true" ID="ID_1289483934" MODIFIED="1488423342668" TEXT="Objekt-Repr&#xe4;s">
<icon BUILTIN="button_ok"/>
<node CREATED="1434128243334" ID="ID_1828331212" MODIFIED="1434128248667" TEXT="Typ-Darstellung"/>
<node CREATED="1434128239517" ID="ID_1886740948" MODIFIED="1434128250041" TEXT="Mapping"/>
<node CREATED="1434128170381" FOLDED="true" ID="ID_976705384" MODIFIED="1488423342550" TEXT="GenNode">
<linktarget COLOR="#ff0033" DESTINATION="ID_976705384" ENDARROW="Default" ENDINCLINATION="10;45;" ID="Arrow_ID_1285375088" SOURCE="ID_553361956" STARTARROW="Default" STARTINCLINATION="-13;-67;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1435421658394" FOLDED="true" ID="ID_1938259420" MODIFIED="1488423308116" TEXT="ID">
<node CREATED="1435421670349" FOLDED="true" ID="ID_1358247529" MODIFIED="1488423308102" TEXT="verwende EntryID">
<icon BUILTIN="button_ok"/>
<node CREATED="1435421678004" FOLDED="true" ID="ID_691179282" MODIFIED="1488423308087" TEXT="Abh&#xe4;ngigkeitsprobleme">
<node CREATED="1435421693260" ID="ID_1314021887" MODIFIED="1435942753226" TEXT="generische ID-Funktionen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1435421739988" ID="ID_405602814" MODIFIED="1435885199446" TEXT="EntryID geh&#xf6;rt in Library">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1435885214592" FOLDED="true" ID="ID_1198930165" MODIFIED="1488423308087" TEXT="sanitise st&#xf6;rt">
<node CREATED="1435885226222" ID="ID_776697956" MODIFIED="1435942745458" TEXT="verschiebe in EntryID">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1435885235583" ID="ID_444167455" MODIFIED="1435942747401" TEXT="verwende Subklasse">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1439842285584" ID="ID_334339765" MODIFIED="1439842291292" TEXT="spezielle Ref-IDs"/>
</node>
<node CREATED="1434128174030" FOLDED="true" ID="ID_1395250463" MODIFIED="1488423308116" TEXT="Variant">
<node CREATED="1435943070542" FOLDED="true" ID="ID_949070153" MODIFIED="1488423308102" TEXT="Wert-Semantik">
<icon BUILTIN="button_ok"/>
<node CREATED="1435943077974" ID="ID_280152814" MODIFIED="1435943080682" TEXT="kopierbar"/>
<node CREATED="1435943081438" ID="ID_159359464" MODIFIED="1435943083738" TEXT="zuweisbar"/>
<node CREATED="1435943085206" FOLDED="true" ID="ID_734188530" MODIFIED="1488423308088" TEXT="const-ness liegt beim User">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. der usage context entscheidet, ob wir einen Wert,
    </p>
    <p>
      eine Referenz oder einen konstanten Wert verwenden
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
<node CREATED="1435943194398" ID="ID_587131941" MODIFIED="1435943203161" TEXT="GenNode gibt Referenz auf Wert"/>
<node CREATED="1435943203662" ID="ID_1772960325" MODIFIED="1435943274743" TEXT="const GenNode gibt const&amp;">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1435943214822" ID="ID_723738462" MODIFIED="1435943233983">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Record selber ist immuable
    </p>
    <p>
      aber hat eine Builder-Mechanik
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1435943121046" ID="ID_16399922" MODIFIED="1435943145278" TEXT="brauche const Variante">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eigentlich fehlte nur die get()-Operation
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1434128217645" FOLDED="true" ID="ID_1790054544" MODIFIED="1488423308116" TEXT="Monade">
<icon BUILTIN="button_cancel"/>
<node CREATED="1435932580854" FOLDED="true" ID="ID_1307223527" MODIFIED="1488423308103" TEXT="Daten einwickeln">
<node CREATED="1435932589853" ID="ID_180643071" MODIFIED="1435932595665" TEXT="ctor forward"/>
<node CREATED="1435932598197" FOLDED="true" ID="ID_951223738" MODIFIED="1488423308088" TEXT="Problem mit copy ctor">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erledigt... &#228;hm vertagt
    </p>
    <p>
      aber nicht wirklich; der workaround k&#246;nnte schon die L&#246;sung sein #963
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1435932667701" FOLDED="true" HGAP="22" ID="ID_1069242347" MODIFIED="1488423308066" TEXT="Copy matcht generischen ctor" VSHIFT="-9">
<icon BUILTIN="idea"/>
<node CREATED="1435932714261" ID="ID_1395890846" MODIFIED="1435932719281" TEXT="gleiches Problem schon bei Variant"/>
<node CREATED="1435932719709" ID="ID_188423010" MODIFIED="1435932783846" TEXT="dort explizit gemacht, da komplex">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ich hatte damals beim Variant und zugeh&#246;rigen Buffer die Sorge,
    </p>
    <p>
      da&#223; ich die Implikationen einer generischen L&#246;sung nicht durchdringen kann.
    </p>
    <p>
      Und ich wollte keine Zeit auf einen exzessiven Unit-Test verwenden
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1435942764328" ID="ID_1740355148" MODIFIED="1435942879414">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      generische L&#246;sung verschoben <font color="#990033">#963</font>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#ff3333" DESTINATION="ID_1935900779" ENDARROW="Default" ENDINCLINATION="188;0;" ID="Arrow_ID_1626382520" STARTARROW="Default" STARTINCLINATION="2;73;"/>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1435942827511" ID="ID_614756812" MODIFIED="1435942844839">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      C++11 erlaubt <b>=default</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1435932611653" ID="ID_1701726752" MODIFIED="1435932628945" TEXT="Ansatz-1 (einfach): explizit"/>
<node CREATED="1435932629517" ID="ID_1935900779" MODIFIED="1435942879415" TEXT="Ansatz-2: Selbst-Typ ausblenden">
<linktarget COLOR="#ff3333" DESTINATION="ID_1935900779" ENDARROW="Default" ENDINCLINATION="188;0;" ID="Arrow_ID_1626382520" SOURCE="ID_1740355148" STARTARROW="Default" STARTINCLINATION="2;73;"/>
</node>
</node>
</node>
<node CREATED="1435942891695" FOLDED="true" ID="ID_947731706" MODIFIED="1488423308103" TEXT="Iteration">
<linktarget COLOR="#98e2df" DESTINATION="ID_947731706" ENDARROW="Default" ENDINCLINATION="-78;95;" ID="Arrow_ID_197324270" SOURCE="ID_1665153106" STARTARROW="None" STARTINCLINATION="168;-25;"/>
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1440983732337" HGAP="35" ID="ID_792682966" MODIFIED="1440983809484" TEXT="monadische Iteration" VSHIFT="12">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nicht klar, ob wir das &#252;berhaupt brauchen
    </p>
    <ul>
      <li>
        entweder nur die unmittelbaren Kinder -&gt; komplexe Logik f&#228;llt auf den Client
      </li>
      <li>
        oder nur die Bl&#228;tter -&gt; man kann die Baum-Struktur nicht wirklich nutzen
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1440983598369" FOLDED="true" ID="ID_1025556053" MODIFIED="1488423308088">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung
    </p>
    <p>
      <font size="1">was wir brauchen</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
<node CREATED="1440983617193" ID="ID_532213208" MODIFIED="1440983627459" TEXT="bracketing"/>
<node CREATED="1440983628399" ID="ID_1711016962" MODIFIED="1440983642457" TEXT="node prefix"/>
<node CREATED="1440983643445" ID="ID_1023025658" MODIFIED="1440983656648" TEXT="depth-first"/>
</node>
<node CREATED="1440983661027" FOLDED="true" ID="ID_507018481" MODIFIED="1488423308088" TEXT="Impl">
<node CREATED="1440983668537" ID="ID_1230038295" MODIFIED="1441936954788" TEXT="IterExplorer verwenden">
<icon BUILTIN="help"/>
</node>
<node CREATED="1440984024736" FOLDED="true" ID="ID_1554494729" MODIFIED="1488423308067" TEXT="Chained Iters">
<node CREATED="1440984028959" ID="ID_896818992" MODIFIED="1440984040210" TEXT="pfiffig"/>
<node CREATED="1440984040870" ID="ID_1008957395" MODIFIED="1440984048057" TEXT="m&#xfc;&#xdf;te der IterIter implementieren"/>
<node CREATED="1440984048380" ID="ID_917358570" MODIFIED="1440984080184">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      geht nicht:
    </p>
    <p>
      rekursiver Abstieg in der Mitte eines Iterators
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node CREATED="1440983855875" FOLDED="true" ID="ID_1991218497" MODIFIED="1488423308067" TEXT="RecursiveSelfIntegration">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das war die Quintessenz der ganzen Entwicklung zum IterExplorer
    </p>
    <p>
      Nachdem ich die depth-first / breadth-first -Strategien systematisch aufgebaut hatte,
    </p>
    <p>
      habe ich das dann reduziert und kompakt nochmal geschrieben.
    </p>
    <p>
      Sehr sch&#246;n!
    </p>
    <p>
      
    </p>
    <p>
      
    </p>
    <p>
      &#252;brigens: genau den verwenden wir auch zur Job-Planung
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-1"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1440983865102" ID="ID_306959180" MODIFIED="1440983871265" TEXT="hoch effizient"/>
<node CREATED="1440983872517" ID="ID_1633584594" MODIFIED="1440983880175" TEXT="pa&#xdf;t genau"/>
<node CREATED="1440983880715" ID="ID_617483189" MODIFIED="1440983893678" TEXT="erfordert speziellen ResultIter"/>
<node COLOR="#999999" CREATED="1440984125546" FOLDED="true" ID="ID_461442477" MODIFIED="1488423307230" TEXT="TODO">
<font NAME="SansSerif" SIZE="10"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1440984130872" FOLDED="true" ID="ID_579146044" MODIFIED="1488423307230" TEXT="ResultIter">
<node CREATED="1440984214126" ID="ID_1124020862" MODIFIED="1441937180496" TEXT="GenNode-Zeiger"/>
<node CREATED="1440984221636" ID="ID_1279195509" MODIFIED="1441937180496" TEXT="Scope-Marker"/>
</node>
<node CREATED="1440984158949" ID="ID_1602941967" MODIFIED="1441937180496" TEXT="BuilderTrait"/>
<node CREATED="1440984166132" FOLDED="true" ID="ID_761913732" MODIFIED="1488423307230" TEXT="explorer Funktion">
<node CREATED="1440984256512" ID="ID_1965762804" MODIFIED="1441937180496" TEXT="verwendet Variant::Visitor"/>
<node CREATED="1440984273357" ID="ID_153302412" MODIFIED="1441937180496" TEXT="steigt in Records ein"/>
</node>
</node>
</node>
<node CREATED="1441935566556" FOLDED="true" ID="ID_128176235" MODIFIED="1488423308067" TEXT="oder doch depthFirst?">
<icon BUILTIN="full-2"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1441935596503" ID="ID_119941709" MODIFIED="1441935613232" TEXT="verwendet einen einfacheren Iterator"/>
<node CREATED="1441935617115" ID="ID_1743908800" MODIFIED="1441935704268" TEXT="hat daf&#xfc;r den Stack (deque) explizit"/>
<node CREATED="1441935708383" ID="ID_1709495985" MODIFIED="1441936067879" TEXT="Erkenntnis: Stack ist unvermeidbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn wir m&#252;ssen den Weg zur&#252;ck finden.
    </p>
    <p>
      Wenn also eine Datenstruktur nur einfach verzeigert ist, oder direkt rekursiv (wie bei uns),
    </p>
    <p>
      dann ist es <i>absolut unm&#246;glich,</i>&#160;eine Traversierung mit konstantem Speicher zu machen.
    </p>
    <p>
      Das geht nur bei einer Struktur mit R&#252;ckreferenzen -- diese enthalten dann n&#228;mlich genau den Speicher,
    </p>
    <p>
      der w&#228;hrend dem Einstieg in die einfach verzeigerte Struktur auf dem Stack liegt. Aber letztere
    </p>
    <p>
      braucht nur eine logarithmische Menge an Speicher, und das auch nur w&#228;hrend der Traversierung.
    </p>
    <p>
      Dies ist die Abw&#228;gung, und darunter l&#228;&#223;t sich nichts weghandeln.
    </p>
    <p>
      
    </p>
    <p>
      Der einzige verbleibende Freiheitsgrad ist, bei einer unmittelbaren rekursiven Programmierung
    </p>
    <p>
      direkt den Prozessor-Stack f&#252;r die Speicherung des R&#252;ckweges mitzuverwenden;
    </p>
    <p>
      in dem Moment, wo ich mich f&#252;r einen Iterator entscheide, ist diese M&#246;glichkeit weg.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1441936092427" ID="ID_199084223" MODIFIED="1441936126886">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann genauso effizient werden
    </p>
    <p>
      aber nur, wenn man die Initialisierung hinbekommt
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1441936132022" ID="ID_183615098" MODIFIED="1441936141832" TEXT="trotzdem mu&#xdf; man die Funktion speichern"/>
<node CREATED="1441936142563" ID="ID_161796731" MODIFIED="1441936150975" TEXT="und wir brauchen gar keine flexible Funktion"/>
</node>
<node CREATED="1441936154834" FOLDED="true" ID="ID_1320687783" MODIFIED="1488423308067">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      oder diese Logik
    </p>
    <p>
      fest verdrahten
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-3"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1441936260132" ID="ID_600953695" MODIFIED="1441936270614" TEXT="der IterExplorer zeigt genau, was zu tun ist"/>
<node CREATED="1441936271274" ID="ID_567626167" MODIFIED="1441936284756" TEXT="aber das mehrfache Kopieren zur Initialisierung entf&#xe4;llt"/>
<node CREATED="1441936285368" ID="ID_1119180152" MODIFIED="1441936296746" TEXT="wir brauchen genauso einen ma&#xdf;geschneiderten Scope-Iterator"/>
<node CREATED="1441936356831" ID="ID_1423049913" MODIFIED="1441936372500" TEXT="aber die Indirektion f&#xfc;r die Funktion f&#xe4;llt weg">
<icon BUILTIN="ksmiletris"/>
</node>
<node COLOR="#338800" CREATED="1441936386803" FOLDED="true" ID="ID_588658088" MODIFIED="1488423307235" TEXT="TODO">
<icon BUILTIN="button_ok"/>
<node CREATED="1441936422725" ID="ID_1581368426" MODIFIED="1441936433181" TEXT="ScopeExplorer-Mechanismus mit Stack">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1441936434644" FOLDED="true" ID="ID_455233841" MODIFIED="1488423307234" TEXT="innerer Iterator">
<icon BUILTIN="button_ok"/>
<node CREATED="1441936451042" ID="ID_1773948072" MODIFIED="1441936476114" TEXT="disjunktiver Typ"/>
<node CREATED="1441936869273" ID="ID_864580946" MODIFIED="1441936878867" TEXT="entweder GenNode, oder Rec"/>
<node CREATED="1441936494164" ID="ID_1837843513" MODIFIED="1441936749098" TEXT="Variant ohne Selector?">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da es sich um einen disjunktiven Typ (entweder-oder-Typ) handelt,
    </p>
    <p>
      k&#246;nnte man die Storage mit beiden Bedeutungen &#252;berlagern.
    </p>
    <p>
      
    </p>
    <p>
      Voraussetzung w&#228;re, da&#223; man anhand der konkreten Daten <b>gefahrlos</b>&#160; jeweils herausfinden kann,
    </p>
    <p>
      welcher Zweig grade gilt. Da wir aber keine Introspektion haben (und auch nicht wollen!),
    </p>
    <p>
      w&#252;rde das auf Taschenspielertricks mit der Implementierung hinauslaufen
    </p>
    <ul>
      <li>
        GenNode und Record beginnen beide fraktisch mit einem String. Man m&#252;&#223;te diesen interpretieren k&#246;nnen
      </li>
      <li>
        oder man nutzt die letzten Bits des Pointers, um sich dort eine Flag zu speichern...
      </li>
    </ul>
    <p>
      Damit ist schon klar: <i>sowas macht man nicht ohne Grund</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1441936883063" ID="ID_428834793" MODIFIED="1441936894393" TEXT="GenNode erfordert nur einen Pointer">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1441936900076" FOLDED="true" ID="ID_1582043438" MODIFIED="1488423307234" TEXT="Explorer-Funktion">
<node CREATED="1441936911211" ID="ID_722452490" MODIFIED="1441936917230" TEXT="kommt in DataCap"/>
<node CREATED="1441936983313" ID="ID_1843183144" MODIFIED="1441936990892" TEXT="mu&#xdf; den inneren Iterator liefern"/>
<node CREATED="1441937000487" ID="ID_312630797" MODIFIED="1441937028412">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung: <i>falls</i>&#160;eingebetteter Record
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1441937037498" ID="ID_1964865459" MODIFIED="1443147539495" TEXT="Initialisierung bedenken">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node CREATED="1443147571765" FOLDED="true" ID="ID_1651952492" MODIFIED="1488423308088" TEXT="HierarchyOrientation">
<node CREATED="1443147582018" ID="ID_1791945362" MODIFIED="1443147603945" TEXT="fest einbauen">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1443147586241" ID="ID_904848069" MODIFIED="1443147706043" TEXT="besser: nur level zug&#xe4;nglich machen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Begr&#252;ndung: das Durchlaufen und Rekonstruieren eines Baumes
    </p>
    <p>
      ist letztlich doch ein sehr spezieller Fall, und rechtfertigt nicht,
    </p>
    <p>
      den HierarchyOrientationIndicator in jeden Iterator einzubetten.
    </p>
    <p>
      Zumal -- wenn der level zug&#228;nglich ist -- kann man diese Mechanik genauso gut
    </p>
    <p>
      dort direkt ansiedeln, wo sie gebraucht wird.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1443147730941" ID="ID_196989292" MODIFIED="1443147751954">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      also <i>keine</i>&#160;Monade
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1440984348699" FOLDED="true" HGAP="64" ID="ID_1327214042" MODIFIED="1488423308117" TEXT="Pr&#xe4;dikate" VSHIFT="-26">
<icon BUILTIN="button_ok"/>
<node CREATED="1440984366937" ID="ID_1806640214" MODIFIED="1440984465790">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Gleichheit
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1440984381982" ID="ID_187772178" MODIFIED="1440984463833" TEXT="Wert-Match">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1440984392453" ID="ID_1665153106" MODIFIED="1443147806504" TEXT="Contains">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kombiniert den Wert-Match mit der Iteration
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#98e2df" DESTINATION="ID_947731706" ENDARROW="Default" ENDINCLINATION="-78;95;" ID="Arrow_ID_197324270" STARTARROW="None" STARTINCLINATION="168;-25;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1434128176918" FOLDED="true" ID="ID_863330674" MODIFIED="1488423308117" TEXT="Record">
<icon BUILTIN="button_ok"/>
<node CREATED="1434128198957" FOLDED="true" ID="ID_1224215957" MODIFIED="1488423308103" TEXT="Konstuktor">
<icon BUILTIN="button_ok"/>
<node CREATED="1434421381345" ID="ID_752165044" MODIFIED="1436042396321" TEXT="DSL zur Daten-Definition">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1434421403406" ID="ID_1085825017" MODIFIED="1434421414073" TEXT="Alternative zur Diff-Repr&#xe4;sentation"/>
<node CREATED="1434421422582" ID="ID_1730569377" MODIFIED="1434421448187">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Zweck: kompaktes Anschreiben
    </p>
    <p>
      von literalen Daten
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1435973418262" FOLDED="true" ID="ID_1847939996" MODIFIED="1488423308088">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Object <b>builder</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1435973448902" FOLDED="true" ID="ID_1729239555" MODIFIED="1488423308067" TEXT="wie definieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem ist, wir definieren den Typ Record generisch,
    </p>
    <p>
      verwenden dann aber nur die Spezialisierung Record&lt;GenNode&gt;
    </p>
    <p>
      Und die Builder-Funktionen brauchen eigentlich spezielles Wissen &#252;ber den zu konstruierenden Zieltyp
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1436042433806" ID="ID_601307933" MODIFIED="1436042451130" TEXT="Erweiterungspunkt"/>
<node CREATED="1436042451694" ID="ID_1270184731" MODIFIED="1436042458786" TEXT="durch explizite Spezialiserung"/>
<node CREATED="1436042460726" ID="ID_818066421" MODIFIED="1436042468802" TEXT="nur f&#xfc;r genNode()"/>
<node CREATED="1436042480886" ID="ID_1194557524" MODIFIED="1436042502641">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Mutator selber is <i>noncopyable</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1435973566277" FOLDED="true" ID="ID_1320441333" MODIFIED="1488423308067">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ergebnis move
    </p>
    <p>
      <font size="1">pro / contra</font>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Move ist <b>gef&#228;hrlich </b>
    </p>
    <p>
      aber auch deutlich effizienter,
    </p>
    <p>
      denn wir m&#252;ssen sonst das ganze erzeugte Ergebnis einmal kopieren.
    </p>
    <p>
      Nicht sicher, ob der Optimiser das hinbekommt
    </p>
  </body>
</html></richcontent>
<node CREATED="1436042507254" ID="ID_1832904297" MODIFIED="1436042518877">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>nur</i>&#160;auf dem Mutator
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1436042520942" ID="ID_1700762999" MODIFIED="1436042539843">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      dieser ist nicht kopierbar
    </p>
    <p>
      und mu&#223; dediziert erstellt werden
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1436042542502" ID="ID_890092502" MODIFIED="1436042552578" TEXT="move passiert immer explizit"/>
</node>
</node>
</node>
<node CREATED="1436042774669" FOLDED="true" ID="ID_714336641" MODIFIED="1488423308103" TEXT="Implementierung">
<icon BUILTIN="button_ok"/>
<node CREATED="1436042783700" ID="ID_381817780" MODIFIED="1436042788584" TEXT="zwei Collections"/>
<node CREATED="1436042814044" ID="ID_1455779230" MODIFIED="1436042818312" TEXT="aber semantisch eine Liste"/>
<node CREATED="1436924462201" FOLDED="true" ID="ID_1369837914" MODIFIED="1488423308088" TEXT="Probleme">
<icon BUILTIN="button_ok"/>
<node CREATED="1436924502056" FOLDED="true" ID="ID_1085481788" MODIFIED="1488423308068" TEXT="R&#xfc;ckgabetyp von Attribut-Gettern">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      m&#246;glicherweise schon gel&#246;st,
    </p>
    <p>
      denn Record ist insgesamt immutable.
    </p>
    <p>
      Also k&#246;nnen wir einen Find mit einem const_iterator machen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1439826834113" ID="ID_1331810475" MODIFIED="1439826858261">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      was sinnvoll ist,
    </p>
    <p>
      h&#228;ngt vom Payload-Typ ab
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439826859630" ID="ID_658403513" MODIFIED="1439826896096">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bei einer 'key = value' -Syntax mit strings
    </p>
    <p>
      ist nur ein Value-R&#252;ckgabewert sinnvoll
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439826896985" ID="ID_180703618" MODIFIED="1439826904996" TEXT="habe es daher generisch/konfigurierbar gemacht"/>
<node CREATED="1439826905728" ID="ID_955222288" MODIFIED="1439826937458" TEXT="-&gt; &#xfc;berhaupt eine Typkonfiguration ist sinnvoll">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...auch kann man auf diesem Weg die Storage konfigurierbar machten
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1436924529208" FOLDED="true" ID="ID_1300693998" MODIFIED="1488423308068" TEXT="Handhabung des Typ-Feldes">
<icon BUILTIN="button_ok"/>
<node CREATED="1436925315589" FOLDED="true" ID="ID_327577903" MODIFIED="1488423307246" TEXT="herausfiltern">
<icon BUILTIN="help"/>
<node CREATED="1439826509444" ID="ID_333663358" MODIFIED="1439826524386" TEXT="ja">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439826512572" ID="ID_1522833188" MODIFIED="1439826522070" TEXT="aus beliebigem Attribut"/>
</node>
<node CREATED="1436924568352" FOLDED="true" ID="ID_1243616839" MODIFIED="1488423307246" TEXT="in Attribut-Iterator">
<icon BUILTIN="help"/>
<node CREATED="1439826529649" ID="ID_1514781996" MODIFIED="1439826559628" TEXT="w&#xe4;re sch&#xf6;n">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1439826535625" ID="ID_872404626" MODIFIED="1439826563061" TEXT="ist aber aufwendig">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1439826545167" ID="ID_185501908" MODIFIED="1439826726913" TEXT="geht nur mit Metadata-Collection">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da wir einen IterAdapter verwenden, k&#246;nnen wir nur eine 'pos' (einen Quell-Iterator)
    </p>
    <p>
      als Zustands-Markierung verwenden; die gleiche 'pos' wird aber auch inkrementiert und dereferenziert.
    </p>
    <p>
      Daher ist die einzige praktikable L&#246;sung, da&#223; die Typ-ID in einem weiteren Vektor gespeichert wird.
    </p>
    <p>
      Das k&#246;nnte dann ein Metadaten-Vektor sein.
    </p>
    <p>
      
    </p>
    <p>
      Nat&#252;rlich ist dieser Ansatz nur sinnvoll, <i>wenn wir wirklich Metadaten brauchen.</i>
    </p>
    <p>
      Denn jeder Record zahlt den Preis f&#252;r die komplexere (zus&#228;tzliche) Datenstruktur!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
</node>
<node CREATED="1436924578936" FOLDED="true" ID="ID_677097690" MODIFIED="1488423307246" TEXT="in Attribut-Collection ablegen">
<icon BUILTIN="help"/>
<node CREATED="1439826758699" ID="ID_873913305" MODIFIED="1439826761999" TEXT="pfiffige Idee"/>
<node CREATED="1439826762731" ID="ID_339809602" MODIFIED="1439826766758" TEXT="aber eigentlich ein Pfusch"/>
<node CREATED="1439826767274" ID="ID_1506445345" MODIFIED="1439826777093" TEXT="man m&#xfc;&#xdf;te jede Eingans-Collection normalisieren"/>
<node CREATED="1439826782368" ID="ID_386953463" MODIFIED="1439826789763" TEXT="man m&#xfc;&#xdf;te Duplikate filtern"/>
<node CREATED="1439826792343" ID="ID_775227666" MODIFIED="1439826804002" TEXT="NJET">
<icon BUILTIN="button_cancel"/>
</node>
</node>
</node>
<node CREATED="1439827003483" FOLDED="true" ID="ID_514691403" MODIFIED="1488423308068" TEXT="&apos;magische&apos; IDs als Attribute">
<icon BUILTIN="button_ok"/>
<node CREATED="1439827021297" ID="ID_154339557" MODIFIED="1439827028484" TEXT="brauche ich f&#xfc;r die Diff-Language"/>
<node CREATED="1439827061051" ID="ID_1563625085" MODIFIED="1439827543682" TEXT="implementiert als spezielle, magische ID-Referenzen">
<arrowlink COLOR="#5bf0d0" DESTINATION="ID_913220298" ENDARROW="Default" ENDINCLINATION="366;-59;" ID="Arrow_ID_806648905" STARTARROW="None" STARTINCLINATION="347;320;"/>
</node>
<node CREATED="1439827599485" ID="ID_426467512" MODIFIED="1439827625675" TEXT="ist es problematisch, wenn solche in die normalen Attribute geraten">
<icon BUILTIN="help"/>
</node>
<node CREATED="1439827636256" ID="ID_339472941" MODIFIED="1439827645763" TEXT="entscheidet sich im Diff-Algorithmus, sonst wurscht"/>
<node CREATED="1439827647038" ID="ID_1668349556" MODIFIED="1439827693504" TEXT="also geh&#xf6;rt das in einen h&#xf6;heren Layer">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1439827663860" ID="ID_582619719" MODIFIED="1439827698437" TEXT="Record sollte sich hier neutral verhalten">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1434128179406" FOLDED="true" HGAP="25" ID="ID_1833179523" MODIFIED="1488423308103" TEXT="Referez" VSHIFT="13">
<icon BUILTIN="button_ok"/>
<node CREATED="1434129158157" FOLDED="true" ID="ID_1777328498" MODIFIED="1488423308088" TEXT="sicher dereferenzierbar">
<node CREATED="1434205928410" ID="ID_733269570" MODIFIED="1434205947253" TEXT="entweder zwangsweise gebunden"/>
<node CREATED="1434205947841" ID="ID_871233558" MODIFIED="1434205955964" TEXT="oder NULL-Zustand mit Exception"/>
<node CREATED="1434205957177" ID="ID_499991180" MODIFIED="1434205968740" TEXT="inherente Unsicherheit einer Referenz"/>
</node>
<node CREATED="1434129167805" FOLDED="true" ID="ID_819452470" MODIFIED="1488423308088" TEXT="stand-in">
<arrowlink COLOR="#00ff33" DESTINATION="ID_654762061" ENDARROW="Default" ENDINCLINATION="-390;37;" ID="Arrow_ID_724106052" STARTARROW="Default" STARTINCLINATION="-48;187;"/>
<icon BUILTIN="help"/>
<node CREATED="1434129196709" ID="ID_1004519740" MODIFIED="1436042656829" TEXT="Subklasse von Rec">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      scheidet aus, wegen Wertsemantik
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1434129204149" FOLDED="true" ID="ID_1688475597" MODIFIED="1488423308068" TEXT="GenNode">
<linktarget COLOR="#66ff66" DESTINATION="ID_1688475597" ENDARROW="Default" ENDINCLINATION="219;91;" ID="Arrow_ID_57985873" SOURCE="ID_60404225" STARTARROW="Default" STARTINCLINATION="23;-52;"/>
<node CREATED="1434205661969" ID="ID_1484374626" MODIFIED="1434205705054">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mit <i>speziellem </i>Ref-Typ
    </p>
    <p>
      <font size="1">-- im DataCap</font>
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#ffcccc" COLOR="#990033" CREATED="1434205598709" FOLDED="true" ID="ID_235720343" MODIFIED="1488423307249" TEXT="stand-in hei&#xdf;t...">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1434205739609" ID="ID_1185983904" MODIFIED="1434205778045" TEXT="kann anstelle eines Objektes treten"/>
<node CREATED="1434205834506" ID="ID_1477654683" MODIFIED="1434205859229" TEXT="transparent f&#xfc;r den Aufrufer"/>
<node CREATED="1434205862449" ID="ID_1736858324" MODIFIED="1434205879837" TEXT="Konsequenz: DataCap mu&#xdf; das verstehen"/>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1434128128869" FOLDED="true" ID="ID_244966341" MODIFIED="1488423342668" TEXT="Verben">
<node CREATED="1434128134508" FOLDED="true" ID="ID_553361956" MODIFIED="1488423342550" TEXT="ID-Repr&#xe4;s">
<arrowlink COLOR="#ff0033" DESTINATION="ID_976705384" ENDARROW="Default" ENDINCLINATION="10;45;" ID="Arrow_ID_1285375088" STARTARROW="Default" STARTINCLINATION="-13;-67;"/>
<node CREATED="1434128393429" ID="ID_1275202366" MODIFIED="1434128584214" TEXT="mu&#xdf; GenNode sein">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1434128412934" FOLDED="true" ID="ID_1319614474" MODIFIED="1488423308117" TEXT="Repr&#xe4;s entscheiden">
<icon BUILTIN="go"/>
<node CREATED="1434128438565" FOLDED="true" ID="ID_913220298" MODIFIED="1488423308103" TEXT="als ID erkennbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: in der Diff-Verarbeitung wird dieser spezielle check verwendet
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#5bf0d0" DESTINATION="ID_913220298" ENDARROW="Default" ENDINCLINATION="366;-59;" ID="Arrow_ID_806648905" SOURCE="ID_1563625085" STARTARROW="None" STARTINCLINATION="347;320;"/>
<node CREATED="1434128740117" ID="ID_1537979881" MODIFIED="1434128764209" TEXT="spezielles Baumuster"/>
<node CREATED="1434128764893" ID="ID_1430586148" MODIFIED="1434128768689" TEXT="Gefahr von clashes"/>
<node CREATED="1434128769325" FOLDED="true" ID="ID_866845827" MODIFIED="1488423308089" TEXT="entscheide">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1434128779661" ID="ID_1739097548" MODIFIED="1434236311060" TEXT="marker-ID + string-Payload">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1434128917125" ID="ID_392407967" MODIFIED="1436021562160" TEXT="&quot;fehlkonstruierte&quot; ID + pr&#xfc;f-Pr&#xe4;dikat">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1434128981381" ID="ID_101281763" MODIFIED="1436042656881" TEXT="spezielle Ref-Payload">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      m.E. die einzig saubere Desgin-Variante!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1436021576224" FOLDED="true" ID="ID_1239136010" MODIFIED="1488423308068" TEXT="Begr&#xfc;ndung">
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="idea"/>
<node CREATED="1436021581655" ID="ID_124352424" MODIFIED="1436021603779" TEXT="hash-identische ID sorgt f&#xfc;r transparente Integration"/>
<node CREATED="1436021610648" ID="ID_1621632066" MODIFIED="1436021618067" TEXT="das nimmt Komplexit&#xe4;t aus der Anwendung heraus"/>
<node CREATED="1436021623799" ID="ID_929212705" MODIFIED="1436021637115" TEXT="f&#xfc;r die Dereferenzierung zahlt nur die Referenz"/>
<node CREATED="1436021694039" ID="ID_1657867881" MODIFIED="1436021817445" TEXT="Nachteil ist Gefahr der Verwirrung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint ist:
    </p>
    <ul>
      <li>
        man kann alternativ auch eine RecordRef direkt in eine elementare GenNode packen
      </li>
      <li>
        diese verh&#228;lt sich dann nicht transparent, denn sie hat eine andere Identit&#228;t als ihr Ziel
      </li>
      <li>
        das kann aber als spezielles Ausdrucksmittel genutzt werden
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1436021719055" ID="ID_1039111553" MODIFIED="1436021730099" TEXT="ist nur ein halber Nachteil"/>
<node CREATED="1436021731615" ID="ID_1576857183" MODIFIED="1436021741970" TEXT="kann n&#xe4;mlich auch Ausdrucksmittel sein"/>
<node CREATED="1436021848790" ID="ID_742066846" MODIFIED="1436021874042" TEXT="problematisch ist die Implementerung des Erkennungs-Pr&#xe4;dikates"/>
</node>
</node>
</node>
<node CREATED="1434128446029" FOLDED="true" ID="ID_1779802587" MODIFIED="1488423308103" TEXT="hash-identisch">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: wird direkt von standard-equality so behandelt
    </p>
  </body>
</html></richcontent>
<node CREATED="1434128685597" ID="ID_690649535" MODIFIED="1434128705631">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      brauche speziellen Builder,
    </p>
    <p>
      der das so fabriziert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1434128706589" FOLDED="true" ID="ID_1001559218" MODIFIED="1488423308089">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bekomme einen
    </p>
    <p>
      &quot;ungenutzten&quot; DataCap
    </p>
  </body>
</html></richcontent>
<node CREATED="1434128996949" ID="ID_654762061" MODIFIED="1434239007746" TEXT="k&#xf6;nnte zur Ref ausgebaut werden">
<linktarget COLOR="#00ff33" DESTINATION="ID_654762061" ENDARROW="Default" ENDINCLINATION="-390;37;" ID="Arrow_ID_724106052" SOURCE="ID_819452470" STARTARROW="Default" STARTINCLINATION="-48;187;"/>
</node>
<node CREATED="1434130839653" FOLDED="true" HGAP="22" ID="ID_60404225" MODIFIED="1488423308068" VSHIFT="8">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Idee: <font color="#990033"><b>Ref-GenNode</b></font>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#66ff66" DESTINATION="ID_1688475597" ENDARROW="Default" ENDINCLINATION="219;91;" ID="Arrow_ID_57985873" STARTARROW="Default" STARTINCLINATION="23;-52;"/>
<icon BUILTIN="idea"/>
<node CREATED="1434130866693" ID="ID_1402852366" MODIFIED="1434130886826">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      als Ref erkennbar
    </p>
    <p>
      <font size="1">(Pr&#228;dikat)</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1434130888581" ID="ID_369455584" MODIFIED="1434130912181">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hash-identische
    </p>
    <p>
      Ziel-ID ableitbar
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1434206472689" ID="ID_561057428" MODIFIED="1434206482829" TEXT="Variant-Subklasse"/>
<node CREATED="1434206483657" ID="ID_473311646" MODIFIED="1434206508445" TEXT="Cast auf Rec-Typ pr&#xfc;fen"/>
<node CREATED="1434206509201" ID="ID_717222987" MODIFIED="1434206522117" TEXT="nur Ref-Fall zahlt Overhead"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1434129043245" FOLDED="true" ID="ID_1120430427" MODIFIED="1488423342550" TEXT="brauchen wir einen ref-Typ">
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="help"/>
<node CREATED="1434129063053" ID="ID_1242923371" MODIFIED="1434129082821">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Verarbeiten
    </p>
    <p>
      von Teilb&#228;umen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1436019533354" ID="ID_1500539399" MODIFIED="1436019542698" TEXT="Konzept-Bruch">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1436042718309" ID="ID_109270255" MODIFIED="1436042725210" TEXT="Ja!">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1443733567706" HGAP="241" ID="ID_143203937" MODIFIED="1473353232804" TEXT="Diff-Language" VSHIFT="23">
<cloud COLOR="#cfba9d"/>
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="go"/>
<node CREATED="1443733726563" ID="ID_410650103" MODIFIED="1443733731422" TEXT="Grundlagen">
<node CREATED="1443733732938" ID="ID_73184558" MODIFIED="1443733737277" TEXT="Folge von Verben"/>
<node CREATED="1443733738296" ID="ID_994909291" MODIFIED="1443733743181" TEXT="konstante Gr&#xf6;&#xdf;e"/>
<node CREATED="1443733744169" ID="ID_1410679171" MODIFIED="1443733756330" TEXT="ein Argument pro Verb"/>
<node CREATED="1443733768621" ID="ID_183648684" MODIFIED="1443733796453">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>Interpreter</b>&#160;definiert Sprache
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#56397a" CREATED="1443733800322" ID="ID_1544242343" MODIFIED="1488423307267" TEXT="Nutz-Muster">
<font NAME="SansSerif" SIZE="13"/>
<node CREATED="1443733822780" FOLDED="true" ID="ID_756812969" MODIFIED="1488423342668" TEXT="Basis-F&#xe4;lle">
<node CREATED="1443733856089" FOLDED="true" ID="ID_1358940500" MODIFIED="1488423342550">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ROOT
    </p>
  </body>
</html></richcontent>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443733978248" ID="ID_724172314" MODIFIED="1443733983252" TEXT="Wurzel pflanzen"/>
<node CREATED="1443733984336" ID="ID_109703386" MODIFIED="1443733993674" TEXT="Au&#xdf;erhalb der Sprache"/>
<node CREATED="1443733994310" ID="ID_52662692" MODIFIED="1443733998417" TEXT="...oder rekursiv"/>
</node>
<node CREATED="1443733868958" FOLDED="true" ID="ID_676848577" MODIFIED="1488423342550">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      INIT
    </p>
  </body>
</html></richcontent>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734005660" FOLDED="true" ID="ID_1028914628" MODIFIED="1488423308117">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      leeres
    </p>
    <p>
      Objekt
    </p>
  </body>
</html></richcontent>
<node CREATED="1443734026466" FOLDED="true" ID="ID_1051774321" MODIFIED="1488423308103" TEXT="typen">
<node CREATED="1443735007685" ID="ID_1426207394" MODIFIED="1443735015668" TEXT="&quot;type&quot;-Attribut">
<font NAME="SansSerif" SIZE="11"/>
</node>
</node>
<node CREATED="1443734030329" ID="ID_257027438" MODIFIED="1443734033052" TEXT="bef&#xfc;llen"/>
</node>
</node>
<node CREATED="1443733903314" FOLDED="true" ID="ID_1973278843" MODIFIED="1488423342550" TEXT="NAV">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734039776" FOLDED="true" ID="ID_1572908212" MODIFIED="1488423308117" TEXT="pick">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1444349667741" ID="ID_1407797227" MODIFIED="1444349672062" TEXT="by-name"/>
<node CREATED="1444349672792" ID="ID_267468753" MODIFIED="1444349690748" TEXT="anonym">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      pick(Ref::CHILD)
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1443734637416" FOLDED="true" ID="ID_461220696" MODIFIED="1488423308117" TEXT="after">
<node CREATED="1443741466072" ID="ID_1108877437" MODIFIED="1444349773888" TEXT="Frage: wrap erlauben">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      w&#252;rde sagen: ja, aber auch <i>nur</i>&#160;f&#252;r das after-Verb!
    </p>
    <p>
      allgemein halte ich einen wrap-around f&#252;r keine gute Idee,
    </p>
    <p>
      weil er zu Zweideutigekeigen f&#252;hrt und daher Struktur oder Konsistenzfehler &#252;berspielt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node CREATED="1443733891492" FOLDED="true" ID="ID_1727724941" MODIFIED="1488423342550" TEXT="MOD">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734646734" FOLDED="true" ID="ID_841341176" MODIFIED="1488423308117" TEXT="INS">
<node CREATED="1443734713781" ID="ID_10743682" MODIFIED="1443734720912" TEXT="vorne"/>
<node CREATED="1443734721692" ID="ID_1206301961" MODIFIED="1443734723047" TEXT="innen"/>
<node CREATED="1443734723899" ID="ID_614189310" MODIFIED="1443734740809" TEXT="APPEND">
<font NAME="SansSerif" SIZE="11"/>
</node>
</node>
<node CREATED="1443734649902" FOLDED="true" ID="ID_1557261619" MODIFIED="1488423308117" TEXT="DEL">
<node CREATED="1443734770118" ID="ID_1863350908" MODIFIED="1443734772777" TEXT="n&#xe4;chsten"/>
<node CREATED="1443734773317" ID="ID_738630660" MODIFIED="1443734775409" TEXT="letzten"/>
</node>
<node CREATED="1443734652509" FOLDED="true" ID="ID_72317753" MODIFIED="1488423308117" TEXT="PERM">
<node CREATED="1443734781868" ID="ID_868966231" MODIFIED="1443734785215" TEXT="find-fetch"/>
<node CREATED="1443734785971" FOLDED="true" ID="ID_1082279397" MODIFIED="1488423308103" TEXT="push">
<icon BUILTIN="button_cancel"/>
<node CREATED="1443734808360" ID="ID_1918783570" MODIFIED="1443734832562" TEXT="Parameter">
<icon BUILTIN="help"/>
</node>
<node CREATED="1443734796346" ID="ID_1017278649" MODIFIED="1443734869522" TEXT="&#xfc;berfl&#xfc;ssig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      l&#228;&#223;t sich stets duch eine inverse Folge von <i>find</i>&#160;und <i>pick</i>&#160; emulieren
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1443733895611" FOLDED="true" ID="ID_1632274940" MODIFIED="1488423342550" TEXT="MUT">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734664572" FOLDED="true" ID="ID_602667530" MODIFIED="1488423308117" TEXT="by-name">
<node CREATED="1443734917130" ID="ID_583238947" MODIFIED="1443734923740" TEXT="nur im Scope"/>
<node CREATED="1443734926912" ID="ID_634765493" MODIFIED="1443734937699" TEXT="er&#xf6;ffnet sub-Scope"/>
</node>
<node CREATED="1443734668267" FOLDED="true" ID="ID_1281243531" MODIFIED="1488423308118" TEXT="_THIS_">
<font NAME="SansSerif" SIZE="11"/>
<node CREATED="1443734884526" ID="ID_1720972097" MODIFIED="1443734907375" TEXT="gilt postfix"/>
<node CREATED="1443734942974" ID="ID_72035627" MODIFIED="1443734946841" TEXT="sonst wie normales MUT"/>
<node CREATED="1443734955012" ID="ID_132520838" MODIFIED="1443734972286" TEXT="erlaubt Anlegen + Populieren"/>
<node CREATED="1460934541540" ID="ID_789170184" MODIFIED="1460934562112" TEXT="von fragw&#xfc;rdigem Nutzen!">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
</node>
<node CREATED="1443735171208" FOLDED="true" HGAP="46" ID="ID_1757651048" MODIFIED="1488423342668" TEXT="Objekt-Baum-Semantik" VSHIFT="13">
<node CREATED="1443735379267" FOLDED="true" ID="ID_628105032" MODIFIED="1488423342550" TEXT="Wurzel">
<node CREATED="1443735418201" ID="ID_1521312079" MODIFIED="1443735418201" TEXT="wird vorausgesetzt"/>
<node CREATED="1443735385523" ID="ID_1179435276" MODIFIED="1443735425593" TEXT="ist stets ein Record"/>
</node>
<node CREATED="1443735531351" FOLDED="true" ID="ID_1428234818" MODIFIED="1488423342551" TEXT="Objekt">
<node CREATED="1443735554379" ID="ID_676366486" MODIFIED="1443735571653" TEXT="Inhalt als Liste"/>
<node CREATED="1443735572833" FOLDED="true" ID="ID_343264561" MODIFIED="1488423308118" TEXT="Zonen">
<node CREATED="1443735596550" ID="ID_236186740" MODIFIED="1443735598258" TEXT="Meta"/>
<node CREATED="1443735598966" ID="ID_1441633165" MODIFIED="1443735601009" TEXT="Attribute"/>
<node CREATED="1443735601477" ID="ID_1740180586" MODIFIED="1443735605393" TEXT="Scope"/>
</node>
<node CREATED="1443735621027" FOLDED="true" ID="ID_850995598" MODIFIED="1488423308118" TEXT="Protokoll">
<node CREATED="1443735625714" ID="ID_1979053137" MODIFIED="1443735638372" TEXT="Reihenfolge erhaltend"/>
<node CREATED="1443735638840" ID="ID_1930287259" MODIFIED="1443735642020" TEXT="nicht sortiert"/>
<node CREATED="1443735643536" ID="ID_1397296376" MODIFIED="1443735648747" TEXT="nach Zonen geordnet"/>
</node>
<node CREATED="1443735736427" FOLDED="true" ID="ID_735577464" MODIFIED="1488423308118" TEXT="Konformit&#xe4;t">
<node CREATED="1443736349465" FOLDED="true" ID="ID_1312270317" MODIFIED="1488423308104" TEXT="strikt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vorerst verworfen, da zus&#228;tzlicher Pr&#252;f-Aufwand
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
<node CREATED="1443736367255" ID="ID_1634633098" MODIFIED="1443736380225" TEXT="Meta-Attribute zuerst"/>
<node CREATED="1443736626731" ID="ID_600165347" MODIFIED="1443736652396" TEXT="keine Attribut-Duplikate"/>
<node CREATED="1443736388180" ID="ID_435516555" MODIFIED="1443736666538" TEXT="Attribute strikt vor Kindern"/>
</node>
<node CREATED="1443736354800" FOLDED="true" ID="ID_1024688383" MODIFIED="1488423308104" TEXT="locker">
<icon BUILTIN="button_ok"/>
<node CREATED="1443735740907" ID="ID_1103250008" MODIFIED="1443735747902" TEXT="Meta-Attribute sind magisch"/>
<node CREATED="1473353076877" FOLDED="true" ID="ID_1936057196" MODIFIED="1488423308089" TEXT="2-Layer binding">
<icon BUILTIN="button_ok"/>
<node CREATED="1473353083492" FOLDED="true" ID="ID_1800912467" MODIFIED="1488423308069" TEXT="es gibt eine Schicht">
<node CREATED="1473353090435" ID="ID_708279376" MODIFIED="1473353094535" TEXT="f&#xfc;r Attribute"/>
<node CREATED="1473353095051" ID="ID_484062615" MODIFIED="1473353096974" TEXT="f&#xfc;r Kinder"/>
</node>
<node CREATED="1473353098274" ID="ID_1387467215" MODIFIED="1473353107805" TEXT="jede Schicht arbeitet in sich konsistent"/>
<node CREATED="1473353109241" ID="ID_132411161" MODIFIED="1473353115884" TEXT="sichtbare Reihenfolge bleibt fest"/>
</node>
<node CREATED="1443736688027" FOLDED="true" ID="ID_1189409086" MODIFIED="1488423308089" TEXT="Attribut-Handhabung">
<node CREATED="1443737483704" FOLDED="true" ID="ID_1359413673" MODIFIED="1488423308069" TEXT="Modell &quot;Liste&quot;">
<icon BUILTIN="button_ok"/>
<node CREATED="1443737510238" ID="ID_1135997794" MODIFIED="1443737510238" TEXT="Duplikate anh&#xe4;ngen"/>
<node CREATED="1443737516852" ID="ID_636329172" MODIFIED="1443737527750" TEXT="Einf&#xfc;gen erlauben"/>
<node CREATED="1443737528578" ID="ID_1554159544" MODIFIED="1443737538461" TEXT="Umordnen erlauben"/>
<node CREATED="1443737570229" ID="ID_28119998" MODIFIED="1443737576392" TEXT="L&#xf6;schen erfordert Ansteuern"/>
</node>
<node CREATED="1443737497870" FOLDED="true" ID="ID_113467015" MODIFIED="1488423308069" TEXT="Modell &quot;Map&quot;">
<icon BUILTIN="button_cancel"/>
<node CREATED="1443737705058" ID="ID_1320189713" MODIFIED="1443737716690" TEXT="Operationen an Storage delegieren"/>
<node CREATED="1443737578803" ID="ID_1670234515" MODIFIED="1443737602756" TEXT="Duplikate &#xfc;berschreiben"/>
<node CREATED="1443737605080" ID="ID_609776485" MODIFIED="1443737739674" TEXT="Neue an Standardort anf&#xfc;gen"/>
<node CREATED="1443737747941" ID="ID_1552219906" MODIFIED="1443737757207" TEXT="Umordnungen verweigern/ignorieren"/>
<node CREATED="1443737769122" ID="ID_697980786" MODIFIED="1443737777276" TEXT="L&#xf6;schen an beliebiger Stelle wirksam"/>
</node>
<node CREATED="1443738082216" FOLDED="true" HGAP="38" ID="ID_870184525" MODIFIED="1488423308069" TEXT="Abw&#xe4;gung" VSHIFT="7">
<icon BUILTIN="button_ok"/>
<node CREATED="1443738097022" ID="ID_1609921484" MODIFIED="1443738178020" TEXT="Modelle schlie&#xdf;en sich aus">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...Grund: sie werden durch einen jeweils komplett anderen Ansatz implementiert
    </p>
    <ul>
      <li>
        &quot;Liste&quot; beruht auf dem Attribut-Iterator und dem Aufbauen einer neuen Attribut-Sammlung
      </li>
      <li>
        &quot;Map&quot; beruht darauf, alle Operationen an die Storage zu delegieren
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738179947" FOLDED="true" ID="ID_1249607715" MODIFIED="1488423307271" TEXT="&quot;Liste&quot;">
<node CREATED="1443738196336" FOLDED="true" ID="ID_855336404" MODIFIED="1488423307271" TEXT="Vorteile">
<node CREATED="1443738556296" ID="ID_340267804" MODIFIED="1443738562763" TEXT="verwendet vorhandene Impl"/>
<node CREATED="1443738563263" ID="ID_583767245" MODIFIED="1443738575817" TEXT="uniforme Semantik der Diff-Sprache"/>
<node CREATED="1443738577781" ID="ID_1622210655" MODIFIED="1443738793488" TEXT="partielle Ordnung darstellbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, man kann Attribute in einer &quot;sinnvoll lesbaren&quot; Ordnung anschreiben
    </p>
    <p>
      und sp&#228;ter angef&#252;gte Attribute bleiben so erkennbar.
    </p>
    <p>
      Vorteilhaft f&#252;r Version-Management
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738653931" ID="ID_604966063" MODIFIED="1443738813866" TEXT="Effizienz des List-Diff">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      profitiert also von allen Verbesserungen des allgemeinen Algorithmus
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738592971" ID="ID_790837479" MODIFIED="1455841731908" TEXT="bei uns: hoch effizient">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &quot;hoch effizient&quot;, unter der Annahme, da&#223; fast immer nur konforme &#196;nderungen kommen.
    </p>
    <p>
      Weil dann n&#228;mlich die in unserer Implementierung ggfs. kostspieligen Umordnungen entfallen,
    </p>
    <p>
      kommen wir auf lineare Komplexit&#228;t f&#252;r die Verarbeitung
    </p>
    <p>
      + NlogN f&#252;r den Index zur Diff-Erzeugung
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1443738199160" FOLDED="true" ID="ID_955544777" MODIFIED="1488423307271" TEXT="Nachteile">
<node CREATED="1443738500000" ID="ID_201650237" MODIFIED="1443738505571" TEXT="Duplikate toleriert"/>
<node CREATED="1443738929854" ID="ID_852940283" MODIFIED="1443739153420" TEXT="sinnloser Index-Aufbau (Diff-Erzeugung)">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      unsere Impl der <i>Diff-Erzeugung&#160;(!)</i>
    </p>
    <p>
      baut einen Index auf (N*logN), um Einf&#252;gungen/Entfernungen zu erkennen und Umordnungs-Suche zu unterst&#252;tzen.
    </p>
    <p>
      Wenn wir aber von ausschlie&#223;lich konformen Operationen ausgehen,
    </p>
    <p>
      wird dieser Index nicht ben&#246;tigt. Leider k&#246;nnen wir das aber nicht garantieren, denn
    </p>
    <p>
      es k&#246;nnte ja zwischenzeitlich ein Attribut gel&#246;scht und dann sp&#228;ter (am Ende) wieder
    </p>
    <p>
      angeh&#228;ngt worden sein, was dann eben doch einen Index erfordert, um einen
    </p>
    <p>
      korrekten Listen-Diff zu erzeugen
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1443738179947" FOLDED="true" ID="ID_1861982554" MODIFIED="1488423307272" TEXT="&quot;Map&quot;">
<node CREATED="1443738196336" FOLDED="true" ID="ID_997066012" MODIFIED="1488423307271" TEXT="Vorteile">
<node CREATED="1443738237731" ID="ID_1438410748" MODIFIED="1443738244389" TEXT="Konformit&#xe4;t garantiert"/>
</node>
<node CREATED="1443738199160" FOLDED="true" ID="ID_571621671" MODIFIED="1488423307272" TEXT="Nachteile">
<node CREATED="1443738352419" ID="ID_439396381" MODIFIED="1443738730869" TEXT="Effizienz der Storage">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. <i>wenn</i>&#160;die Storage hoch-optimiert ist,
    </p>
    <p>
      dann &#252;bertr&#228;gt sich das auf die Diff-Behandlung
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738371545" ID="ID_1959160321" MODIFIED="1443738695968" TEXT="bei uns: voll-Quadratisch">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da wir Attribute in einer Liste speichern,
    </p>
    <p>
      m&#252;ssen wir f&#252;r jede Einf&#252;gung eine vollst&#228;ndige Suche machen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738416083" ID="ID_793860786" MODIFIED="1443738470421" TEXT="erfordert separate Impl">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...gemeint ist: extra, anders als die normale Listenverarbeitung.
    </p>
    <p>
      Auch wenn diese andere Implementierung nur delegiert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738471955" ID="ID_315855253" MODIFIED="1443738478894" TEXT="versteckte 2.Semantik"/>
</node>
</node>
</node>
<node CREATED="1443739215487" FOLDED="true" HGAP="42" ID="ID_1540836182" MODIFIED="1488423308069" TEXT="Entscheidung">
<icon BUILTIN="info"/>
<node CREATED="1443739221631" ID="ID_338047062" MODIFIED="1443739227649" TEXT="&quot;Listen&quot;-Modell"/>
<node CREATED="1443739228542" ID="ID_773218806" MODIFIED="1443739244615" TEXT="eindeutig vorzuziehen"/>
<node CREATED="1443739245483" FOLDED="true" ID="ID_1943112941" MODIFIED="1488423307274" TEXT="zwei Gr&#xfc;nde">
<node CREATED="1443739308651" ID="ID_1436069181" MODIFIED="1443739311142" TEXT="Performance"/>
<node CREATED="1443739311562" ID="ID_1848034442" MODIFIED="1443739314222" TEXT="Wartbarkeit"/>
</node>
<node CREATED="1443739323033" FOLDED="true" ID="ID_1428762486" MODIFIED="1488423307274" TEXT="&quot;zwei Listen&quot;-Modell">
<icon BUILTIN="button_ok"/>
<node CREATED="1443739335823" ID="ID_860198617" MODIFIED="1443739349777" TEXT="intern f&#xfc;hren wir zwei Listen-Diffs durch"/>
<node CREATED="1443739350869" ID="ID_464299464" MODIFIED="1443739368974" TEXT="ein Trennkriterium f&#xfc;hrt zum Umschalten"/>
<node CREATED="1443739399718" ID="ID_686887994" MODIFIED="1443739426315">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      danach noch auftretende Attribute
    </p>
    <p>
      erfordern Sonder-Behandlung,
    </p>
    <p>
      indem sie an die Attributs-Liste angeh&#228;ngt werden
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1443733831475" FOLDED="true" HGAP="1" ID="ID_1421473885" MODIFIED="1488423342669" TEXT="Fehler-F&#xe4;lle" VSHIFT="7">
<node CREATED="1443736788646" FOLDED="true" ID="ID_672669721" MODIFIED="1488423342551" TEXT="formal">
<node CREATED="1443736804803" FOLDED="true" ID="ID_1706990948" MODIFIED="1488423308118" TEXT="Syntax">
<node CREATED="1443737154028" ID="ID_143683919" MODIFIED="1443737159781" TEXT="durch Typsystem unterbunden">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1443738002250" ID="ID_1398802900" MODIFIED="1443738006115" TEXT="sicher">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1443736824112" FOLDED="true" ID="ID_1520561537" MODIFIED="1488423308118" TEXT="unpassende Parameter">
<node CREATED="1443737180297" ID="ID_1748864612" MODIFIED="1443737193859" TEXT="Laufzeit-Check">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1443738011018" ID="ID_1408814832" MODIFIED="1443738023248" TEXT="ggfs unvollst&#xe4;ndig">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1443736846078" FOLDED="true" ID="ID_1354240107" MODIFIED="1488423308118" TEXT="Mismatch am Ziel">
<node CREATED="1443736895663" ID="ID_1436447416" MODIFIED="1443736897539" TEXT="erkennbar"/>
<node CREATED="1443736897983" ID="ID_636884790" MODIFIED="1443736900051" TEXT="unbemerkt"/>
</node>
</node>
<node CREATED="1443736791421" FOLDED="true" ID="ID_23907569" MODIFIED="1488423342551" TEXT="semantisch">
<node CREATED="1443737005833" FOLDED="true" ID="ID_1457267751" MODIFIED="1488423308118" TEXT="INIT aber nicht leer">
<node CREATED="1443737222707" ID="ID_1242625886" MODIFIED="1443737241501" TEXT="sehr gef&#xe4;hrlich bei strikter Konformit&#xe4;t"/>
<node CREATED="1443737242824" ID="ID_461820743" MODIFIED="1443737260482" TEXT="harmlos sonst; wird glattgebogen"/>
<node CREATED="1443737279172" ID="ID_1485388967" MODIFIED="1443737290806" TEXT="kann dann aber zu undefiniertem Zustand f&#xfc;hren"/>
</node>
<node CREATED="1443737017511" FOLDED="true" ID="ID_744413839" MODIFIED="1488423308118" TEXT="INS Attribut-Duplikat">
<node CREATED="1443739508920" ID="ID_914206763" MODIFIED="1443739536363" TEXT="wird toleriert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wegen Entscheidung f&#252;r das &quot;Listen&quot;-Modell zur Attribut-Handhabung
    </p>
  </body>
</html></richcontent>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1443737036108" FOLDED="true" ID="ID_641574289" MODIFIED="1488423308118" TEXT="INS Child in Attribut-Zone">
<node CREATED="1473352901693" ID="ID_1444450254" MODIFIED="1473352907657" TEXT="wird toleriert">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1473352908876" ID="ID_288202266" MODIFIED="1473352998161" TEXT="bricht aus der Reihenfolge aus">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t:
    </p>
    <ul>
      <li>
        es wird einfach vom zust&#228;ndigen Layer (der f&#252;r die Attribute) aufgegriffen
      </li>
      <li>
        es hat keinen Einflu&#223; auf die nach au&#223;en sichtbare Reihenfolge
      </li>
      <li>
        diese Reihenfolge bleibt gruppiert nach Attributen / Kindern
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1443737051419" FOLDED="true" ID="ID_364647539" MODIFIED="1488423308118" TEXT="INS Attribut in Scope">
<node CREATED="1473352922946" ID="ID_23438081" MODIFIED="1473352926387" TEXT="wird toleriert">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1473352927098" ID="ID_1791498124" MODIFIED="1473353001242" TEXT="bricht aus der reihenfolge aus">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1443737064633" FOLDED="true" ID="ID_67819372" MODIFIED="1488423308119" TEXT="PERM-fetch Child in Attribut-Zone">
<icon BUILTIN="button_cancel"/>
<node CREATED="1443739787850" ID="ID_31775669" MODIFIED="1443739831711" TEXT="f&#xfc;hrt zum Fehler wg. &quot;zwei Listen&quot;-Modell">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...da das Kind in der Liste der Attribute n&#228;mlich garnicht gefunden wird
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443739798889" ID="ID_1992435989" MODIFIED="1443739912310" TEXT="Spezialbehandlung am Ende der Attributzone notwendig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wenn wir am Ende der Attribut-Zone stehen,
    </p>
    <p>
      und die n&#228;chste Operation ein fetch eines Kindes ist, m&#252;ssen wir implizit den
    </p>
    <p>
      Wechsel in den Scope vollziehen und die Operation dort ausf&#252;hren.
    </p>
    <p>
      Aber an allen anderen Stellen in der Attribut-Zone ist ein solcher Fetch ein Fehler!
    </p>
  </body>
</html></richcontent>
</node>
<node COLOR="#338800" CREATED="1473353036195" ID="ID_371361214" MODIFIED="1473353045794" TEXT="obsolet durch neue Implementierung">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1443735208090" HGAP="99" ID="ID_117472593" MODIFIED="1488423307279" TEXT="Entscheidungen" VSHIFT="8">
<icon BUILTIN="info"/>
<node CREATED="1443735230768" ID="ID_1544379221" MODIFIED="1443735318881">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      standardm&#228;&#223;ig <b>strikt</b>
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1443735286496" FOLDED="true" ID="ID_1442661683" MODIFIED="1488423342669" TEXT="fuzzy-mode">
<node CREATED="1443740092337" ID="ID_563359094" MODIFIED="1443740142638" TEXT="PICK nicht Vorhandenes"/>
<node CREATED="1443740127021" ID="ID_1839086508" MODIFIED="1443740137239" TEXT="DEL nicht Vorhandenes"/>
<node CREATED="1443740193692" ID="ID_1962095124" MODIFIED="1443740200542" TEXT="FIND nicht Vorhandenes"/>
<node CREATED="1443740261859" ID="ID_96033673" MODIFIED="1443740266837" TEXT="MUT nicht Vorhandenes"/>
<node CREATED="1443740311300" ID="ID_1645049477" MODIFIED="1443740327245" TEXT="PICK wird zu AFTER nicht Folgendes"/>
<node CREATED="1443740353686" ID="ID_1996517233" MODIFIED="1443740369848" TEXT="Rest-&#xdc;berschu&#xdf; am Ende"/>
</node>
<node CREATED="1443735322787" FOLDED="true" ID="ID_1404399616" MODIFIED="1488423342669" TEXT="Bindung">
<node CREATED="1443735336009" ID="ID_1123815488" MODIFIED="1443735347412" TEXT="Diff vs. Daten"/>
<node CREATED="1443735348800" ID="ID_1003830940" MODIFIED="1443735353234" TEXT="erst hier die Semantik"/>
<node CREATED="1443740402472" ID="ID_341902192" MODIFIED="1443740407355" TEXT="&quot;zwei Listen&quot;-Modell"/>
</node>
</node>
</node>
<node CREATED="1443741905157" HGAP="364" ID="ID_1960988662" MODIFIED="1488423307285" TEXT="Diff-Implementierung" VSHIFT="17">
<font NAME="SansSerif" SIZE="16"/>
<icon BUILTIN="go"/>
<node CREATED="1444522932841" HGAP="52" ID="ID_48288704" MODIFIED="1444522994078" TEXT="Architektur" VSHIFT="-5">
<node CREATED="1444522939928" ID="ID_490994681" MODIFIED="1444522986895" TEXT="destruktiv?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1444522961462" FOLDED="true" ID="ID_1164434859" MODIFIED="1488423342669">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      List-Diff
    </p>
    <p>
      als Spezialfall
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1444523008207" FOLDED="true" ID="ID_385890020" MODIFIED="1488423342551" TEXT="Sprache: geht nicht">
<icon BUILTIN="button_cancel"/>
<node CREATED="1444523018726" ID="ID_874967614" MODIFIED="1444523024345" TEXT="Problem DiffStep"/>
<node CREATED="1444523025148" ID="ID_1403562226" MODIFIED="1444523033774" TEXT="ist eingebettet in die Sprache"/>
<node CREATED="1444523034795" ID="ID_1203331178" MODIFIED="1444523041573" TEXT="es gibt keinen gemeinsamen Obertyp"/>
<node CREATED="1444523042138" FOLDED="true" ID="ID_675305475" MODIFIED="1488423308119">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann auch nicht
    </p>
    <p>
      wegen dem Interpreter
    </p>
  </body>
</html></richcontent>
<node CREATED="1444523078581" ID="ID_318811786" MODIFIED="1444523091591" TEXT="liefert Token -&gt; kovariant"/>
<node CREATED="1444523091939" ID="ID_720826260" MODIFIED="1444523103670" TEXT="akzeptiert Interpreter -&gt; kontravariant"/>
</node>
</node>
<node CREATED="1444523111792" FOLDED="true" ID="ID_1544468161" MODIFIED="1488423342551" TEXT="Implementierungs-Ebene">
<icon BUILTIN="button_ok"/>
<node CREATED="1444523131486" ID="ID_37078397" MODIFIED="1444523139993" TEXT="Anwendung setzt nur Vector voraus"/>
<node CREATED="1444523140605" ID="ID_1673268008" MODIFIED="1444523237002">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      leicht auf generischen Container
    </p>
    <p>
      zu verallgemeinern
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1444523168561" ID="ID_1180296210" MODIFIED="1444523205481">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Erkennung hat die Sprache als Parameter,
    </p>
    <p>
      und verwendet sie zur Token-Generierung
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1444523206427" ID="ID_713432486" MODIFIED="1444523230705">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      man kann auch dem List-Detector
    </p>
    <p>
      eine Tree-Diff-Language geben
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
</node>
<node CREATED="1444522881125" ID="ID_172319057" MODIFIED="1444522895130" TEXT="Anwendung">
<node CREATED="1443741923547" ID="ID_1978439060" MODIFIED="1443741930738" TEXT="Listen-Diff">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1443741931858" FOLDED="true" ID="ID_484829805" MODIFIED="1488423342669" TEXT="Baum-Diff">
<icon BUILTIN="button_ok"/>
<node CREATED="1445295424277" FOLDED="true" ID="ID_1084177503" MODIFIED="1488423342551">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font size="5" color="#e97664">Frage</font>: <i>in-Place?</i>
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="13"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1445295501531" FOLDED="true" ID="ID_1542006741" MODIFIED="1488423308119" TEXT="Problem: Teilb&#xe4;ume">
<node CREATED="1445295582392" ID="ID_243770452" MODIFIED="1445295587819" TEXT="Umh&#xe4;ngen von Teilb&#xe4;umen"/>
<node CREATED="1445295588127" ID="ID_1541079773" MODIFIED="1445295598746" TEXT="Abstieg in Teilb&#xe4;ume"/>
</node>
<node CREATED="1445295543653" FOLDED="true" ID="ID_39528388" MODIFIED="1488423308119" TEXT="wie wird das UI-Model verwendet">
<icon BUILTIN="help"/>
<node CREATED="1445295714215" ID="ID_472108592" MODIFIED="1445295722937" TEXT="um Diffs aufzuspielen"/>
<node CREATED="1445295726940" ID="ID_1067776624" MODIFIED="1445295736327" TEXT="um UI-Elemente anzusprechend"/>
<node CREATED="1445295737436" FOLDED="true" ID="ID_1529650757" MODIFIED="1488423308104" TEXT="f&#xfc;r Callbacks vom UI">
<node CREATED="1445299235061" ID="ID_336159819" MODIFIED="1445299267675">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>entscheidende Frage</b>: wie addressieren?
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1445299271761" ID="ID_1895578975" MODIFIED="1445299278275" TEXT="Bus-Term hat eine ID..."/>
</node>
</node>
<node CREATED="1445299292301" FOLDED="true" HGAP="134" ID="ID_1169271905" MODIFIED="1488423308119" TEXT="pro" VSHIFT="12">
<node CREATED="1445299310971" ID="ID_1955674144" MODIFIED="1445299339570" TEXT="vermeidet Kopieren"/>
<node CREATED="1445299340375" ID="ID_485304706" MODIFIED="1445299346562" TEXT="speziell der nicht betroffenen Teile"/>
<node CREATED="1445299347173" ID="ID_232158633" MODIFIED="1445299356224" TEXT="das ist genau der Sinn des Diff"/>
</node>
<node CREATED="1445299298932" FOLDED="true" HGAP="127" ID="ID_602638586" MODIFIED="1488423308119" TEXT="contra" VSHIFT="-5">
<node CREATED="1445299306659" ID="ID_1695051638" MODIFIED="1445299308751" TEXT="Locking"/>
<node CREATED="1445389209031" ID="ID_1394943777" MODIFIED="1445389223268" TEXT="GenNode ist ein value type"/>
</node>
<node CREATED="1445389251289" FOLDED="true" HGAP="39" ID="ID_1700933130" MODIFIED="1488423308119" TEXT="Entscheidung" VSHIFT="28">
<font NAME="SansSerif" SIZE="15"/>
<node CREATED="1445389298770" FOLDED="true" ID="ID_1935262520" MODIFIED="1488423308104" TEXT="Kompromi&#xdf;">
<icon BUILTIN="help"/>
<node CREATED="1445389275998" FOLDED="true" ID="ID_1081466244" MODIFIED="1488423308089" TEXT="intern kopieren">
<node CREATED="1445389355802" ID="ID_745348631" MODIFIED="1445389365541" TEXT="wir arbeiten auf einer interen Kopie"/>
<node CREATED="1445389366298" ID="ID_432143676" MODIFIED="1445389372460" TEXT="diese geh&#xf6;rt der Implementierung"/>
<node CREATED="1445389373168" ID="ID_1925613497" MODIFIED="1445389404465">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und wird durch die Diff-Anwendung <i>konsumiert</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1445389291404" FOLDED="true" ID="ID_309801260" MODIFIED="1488423308089" TEXT="aber move-ctor anbieten">
<node CREATED="1445389423050" ID="ID_802906323" MODIFIED="1445389433477" TEXT="zur Initialisierung ein bestehendes Objekt &#xfc;bernehmen"/>
<node CREATED="1445389434753" ID="ID_532431684" MODIFIED="1445389454234" TEXT="das Ergebnis in ein bestehendes Objekt schieben"/>
</node>
</node>
<node CREATED="1445550734547" FOLDED="true" ID="ID_960791539" MODIFIED="1488423308104" TEXT="oder doch den Mutator erweitern">
<icon BUILTIN="yes"/>
<node CREATED="1445550748169" ID="ID_1764723062" MODIFIED="1445550752365" TEXT="Vorteil: explizit"/>
<node CREATED="1445550752832" ID="ID_1506018584" MODIFIED="1445550807621" TEXT="Manipulation hat gewisse Vorteile"/>
<node CREATED="1445550808281" ID="ID_1970167609" MODIFIED="1445550842143">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>Immutablility</i>&#160;erzwingt
    </p>
    <ul>
      <li>
        persistente Datenstrukturen
      </li>
      <li>
        garbage-collector
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1445555721981" FOLDED="true" ID="ID_292803227" MODIFIED="1488423308104">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      L&#246;sung: wir arbeiten <i>auf </i>einem Mutator
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1445556525714" ID="ID_1223708278" MODIFIED="1445556595037" TEXT="damit funktioniert es 1:1 wie List-Diff">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1445556567980" ID="ID_783100840" MODIFIED="1445556591370" TEXT="Voraussetzung: der Mutator mu&#xdf; die Attribut/Kinder-Listen exponieren">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1445644203808" ID="ID_1105228896" MODIFIED="1445644221661">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      auf dem Umweg &#252;ber einen <b>ContentMutator</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1445550873904" ID="ID_1781797833" MODIFIED="1445556509368" TEXT="copy + transaktional als optionaler 2.Layer"/>
<node CREATED="1445550893222" ID="ID_2118555" MODIFIED="1445550915111" TEXT="Diff-Applikator = destruktiver Mutator"/>
<node CREATED="1445389311690" ID="ID_1926972913" MODIFIED="1445389323819" TEXT="und dieses rekursiv..."/>
</node>
</node>
<node COLOR="#391f9e" CREATED="1445391990778" FOLDED="true" HGAP="148" ID="ID_464295846" MODIFIED="1488423342551" TEXT="Implementierung" VSHIFT="4">
<icon BUILTIN="button_ok"/>
<node CREATED="1445392080175" ID="ID_341024968" MODIFIED="1445392101243">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Innereien des alten Record <i>verbrauchen</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1445392105075" ID="ID_1377567733" MODIFIED="1445392113677" TEXT="zwei neue, lokale Vektoren aufbauen"/>
<node CREATED="1445392114217" ID="ID_71124283" MODIFIED="1445392120973" TEXT="daraus den neuen Record konstruieren"/>
<node CREATED="1445556634395" FOLDED="true" ID="ID_1143865339" MODIFIED="1488423308119" TEXT="technische Komplikation: &quot;2 Listen&quot;-Modell">
<icon BUILTIN="button_ok"/>
<node CREATED="1445620914088" ID="ID_1919426539" MODIFIED="1445621018036" TEXT="Verhalten &#xe4;ndert sich je nach Scope">
<icon BUILTIN="info"/>
</node>
<node CREATED="1445620938421" ID="ID_557124057" MODIFIED="1445621022038" TEXT="Verzweigung in jedem Diff-Fall">
<icon BUILTIN="info"/>
</node>
<node CREATED="1445620947332" ID="ID_1684844100" MODIFIED="1445621010162" TEXT="Code-Redundanz">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1445620961570" ID="ID_965863635" MODIFIED="1445621006245" TEXT="in Wrapper wegpacken!">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1445620972825" ID="ID_1467461119" MODIFIED="1445621002208" TEXT="schon in den Record::Mutator einbinden">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1445620991326" ID="ID_513677356" MODIFIED="1445620998003" TEXT="eigener Test">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1473352753713" FOLDED="true" HGAP="10" ID="ID_1353201178" MODIFIED="1488423308104" TEXT="besserer Ansatz" VSHIFT="22">
<icon BUILTIN="idea"/>
<node CREATED="1473352767432" ID="ID_1220456854" MODIFIED="1473352784089" TEXT="als interne Repr&#xe4;sentation klassifizieren"/>
<node CREATED="1473352784533" ID="ID_1907760521" MODIFIED="1473352794511" TEXT="das normale Binding f&#xfc;r opaque Datenstrukturen verwenden"/>
<node CREATED="1473352794988" ID="ID_652168710" MODIFIED="1473352818517" TEXT="vorkonfiguriert: 2-Layer Binding"/>
<node CREATED="1473352820096" ID="ID_329351043" MODIFIED="1473352835764" TEXT="Spezialf&#xe4;lle aus der Sprache entfernen">
<icon BUILTIN="button_ok"/>
<icon BUILTIN="yes"/>
</node>
<node COLOR="#338800" CREATED="1473352801547" ID="ID_1001891556" MODIFIED="1473352809203" TEXT="integriert in generischen Diff-Applikator">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1446159438278" FOLDED="true" HGAP="29" ID="ID_563496669" MODIFIED="1488423308120" VSHIFT="8">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem: <b><font color="#ed1c02" size="4">Rekursion</font></b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<icon BUILTIN="clanbomber"/>
<node CREATED="1446159517914" ID="ID_1749096609" MODIFIED="1446159530661" TEXT="wir m&#xfc;ssen rekursiv in Sub-Scope einsteigen"/>
<node CREATED="1446159531745" ID="ID_1660140345" MODIFIED="1446159550795" TEXT="eingeschachtelt wieder ein Record::Mutator"/>
<node CREATED="1446159551718" ID="ID_608514705" MODIFIED="1446159563593" TEXT="Wohin mit der Storage?"/>
<node CREATED="1446159564725" FOLDED="true" ID="ID_893302390" MODIFIED="1488423308104" TEXT="zwei L&#xf6;sungsans&#xe4;tze">
<icon BUILTIN="info"/>
<node CREATED="1446159584034" FOLDED="true" ID="ID_1845322273" MODIFIED="1488423308089" TEXT="rekursiv konsumieren">
<node CREATED="1446159632603" ID="ID_1950074103" MODIFIED="1446160677236">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn ein MUT kommt
    </p>
    <p>
      erzeugt man lokal einen DiffApplikator f&#252;r den geschachtelten Kontext
    </p>
    <p>
      und gibt ihm rekursiv den Diff hinein. Wenn dieser Aufruf zur&#252;ckkehrt
    </p>
    <p>
      ist der gesammte Diff f&#252;r den eingeschachtelten Kontext konsumiert
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1446159744053" FOLDED="true" ID="ID_1735127502" MODIFIED="1488423308069" TEXT="pro">
<node CREATED="1446159775161" ID="ID_192461503" MODIFIED="1446159832244" TEXT="elegant, selbst&#xe4;hnlich"/>
<node CREATED="1446159790702" ID="ID_905335387" MODIFIED="1446159823622" TEXT="sauber, state bleibt lokal"/>
<node CREATED="1446159833753" ID="ID_539121048" MODIFIED="1446159854442" TEXT="kein expliziter Stack, Daten auf dem Callstack"/>
</node>
<node CREATED="1446159751068" FOLDED="true" ID="ID_1466026714" MODIFIED="1488423308069" TEXT="contra">
<node CREATED="1446159936323" ID="ID_1344861000" MODIFIED="1446159961793" TEXT="keine einfache Iteration mehr"/>
<node CREATED="1446159910374" ID="ID_1307975840" MODIFIED="1446159998967" TEXT="man mu&#xdf; das Token anschauen"/>
<node CREATED="1446159999939" ID="ID_1385561919" MODIFIED="1446160014569" TEXT="nicht kompatibel mit userem &quot;Interpreter&quot;-Modell"/>
<node CREATED="1446159858701" ID="ID_751122481" MODIFIED="1446160039239" TEXT="Applikator mu&#xdf; selber Diff konsumieren"/>
</node>
</node>
<node CREATED="1446159589369" FOLDED="true" ID="ID_1854873736" MODIFIED="1488423308089" TEXT="internen Stack">
<node CREATED="1446160049196" ID="ID_893515494" MODIFIED="1446160680983">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn ein MUT kommt,
    </p>
    <p>
      pusht der Applikator seinen privaten Zustand
    </p>
    <p>
      auf einen explizit im Heap verwalteten std::stack
    </p>
    <p>
      und legt einen neuen Mutator an f&#252;r den nested scope
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1446160209758" FOLDED="true" ID="ID_1949126696" MODIFIED="1488423308069" TEXT="pro">
<node CREATED="1446160223212" ID="ID_189646771" MODIFIED="1446160248517" TEXT="kompatibel mit unserem &quot;Interpreter&quot;-Modell"/>
<node CREATED="1446160249609" ID="ID_1437127411" MODIFIED="1446160256555" TEXT="Diff wird einfach weiter iteriert"/>
</node>
<node CREATED="1446160212502" FOLDED="true" ID="ID_572617599" MODIFIED="1488423308070" TEXT="contra">
<node CREATED="1446160522212" ID="ID_1824677628" MODIFIED="1446160588767" TEXT="wir manipulieren state in einer Komponente"/>
<node CREATED="1446160621127" ID="ID_391572254" MODIFIED="1446160635561" TEXT="diese Manipulation passiert mitten aus einem Token-Aufruf"/>
<node CREATED="1446160592347" ID="ID_126615043" MODIFIED="1446160608364" TEXT="Storage-Management ist verwirrend"/>
</node>
</node>
</node>
<node CREATED="1446168257072" HGAP="81" ID="ID_91568209" MODIFIED="1446168481874" VSHIFT="5">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung:
    </p>
    <p>
      interner Stack
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....begr&#252;ndet duch die generische Architektur.
    </p>
    <p>
      Die Trennung von Diff-Iteration und dem Interpreter erm&#246;glicht verschiedene Sprach-Ebenen.
    </p>
    <p>
      Allerdings werde ich f&#252;r die Anwendung auf konkrete Datenstrukturen,
    </p>
    <p>
      also den TreeMutator, vermutlich das andere Modell (rekursiv konsumieren) verwenden.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1446356556349" FOLDED="true" ID="ID_1068649765" MODIFIED="1488423342551" TEXT="offen...">
<node COLOR="#338800" CREATED="1446356765393" ID="ID_293921295" MODIFIED="1473352607814" TEXT="IDs in Testdaten f&#xfc;r GenNodeBasic_test">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem sind mal wieder die automatisch generierten IDs.
    </p>
    <p>
      Die sind nat&#252;rlich anders, wenn wir die ganze Testsuite ausf&#252;hren...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446356588505" FOLDED="true" ID="ID_895292312" MODIFIED="1488423308120" TEXT="gr&#xfc;ndlicher Test">
<icon BUILTIN="yes"/>
<node CREATED="1473352634730" ID="ID_955392554" MODIFIED="1473352641908" TEXT="bis jetzt nur &quot;positiv-Tests&quot;"/>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1473352642473" ID="ID_839131437" MODIFIED="1473352654368" TEXT="Abdeckung der Grenzf&#xe4;lle">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1444522895718" ID="ID_598781690" MODIFIED="1444522897802" TEXT="Generierung">
<node CREATED="1443741923547" ID="ID_1091408512" MODIFIED="1443741930738" TEXT="Listen-Diff">
<icon BUILTIN="button_ok"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1443741931858" ID="ID_826609799" MODIFIED="1443741945481" TEXT="Baum-Diff">
<icon BUILTIN="flag"/>
</node>
</node>
</node>
<node CREATED="1460934597764" HGAP="163" ID="ID_1014666775" MODIFIED="1488423307306" TEXT="Review" VSHIFT="6">
<font NAME="SansSerif" SIZE="15"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1460934670043" ID="ID_1299827687" MODIFIED="1460934677134" TEXT="sp&#xe4;ter zu &#xfc;berpr&#xfc;fen">
<node CREATED="1460934686017" ID="ID_900313780" MODIFIED="1460934697931" TEXT="Klarheit"/>
<node CREATED="1460934698430" ID="ID_1527149180" MODIFIED="1460934702241" TEXT="praktischer Kontext"/>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1460934704022" ID="ID_369718039" MODIFIED="1460934723329" TEXT="fragw&#xfc;rdig">
<icon BUILTIN="help"/>
<node CREATED="1460934726315" FOLDED="true" ID="ID_865694069" MODIFIED="1488423342669" TEXT="Immutabilit&#xe4;t">
<node CREATED="1460934748064" ID="ID_321786426" MODIFIED="1460934753060" TEXT="keine klare Linie"/>
<node CREATED="1460934753440" ID="ID_44139402" MODIFIED="1460934764386" TEXT="einerseits ja, feste Datenwerte"/>
<node CREATED="1460934764830" ID="ID_594837209" MODIFIED="1460934769313" TEXT="dann aber doch Assignment"/>
<node CREATED="1460934771253" ID="ID_1299180901" MODIFIED="1460934782511" TEXT="und ein Gemurkse bei den Records"/>
</node>
<node CREATED="1460934785067" FOLDED="true" ID="ID_945933568" MODIFIED="1488423342669" TEXT="Abk&#xfc;rzungen in der Sprache">
<node CREATED="1460934793738" ID="ID_1602968303" MODIFIED="1460934798453" TEXT="Ref::CHILD"/>
<node CREATED="1460934799137" ID="ID_1189559717" MODIFIED="1460934801653" TEXT="Ref::THIS"/>
<node CREATED="1460934802385" ID="ID_961490107" MODIFIED="1460934807556" TEXT="Ref::ATTRIBS"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1443740543812" HGAP="17" ID="ID_1766169268" MODIFIED="1488423307314" TEXT="Tests" VSHIFT="20">
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="bell"/>
<node CREATED="1443740549220" ID="ID_167459949" MODIFIED="1443740564198" TEXT="List-Diff">
<icon BUILTIN="button_ok"/>
<node CREATED="1443740554315" ID="ID_1970336632" MODIFIED="1443740562083" TEXT="Anwenden">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1443740556587" ID="ID_1725890953" MODIFIED="1443740560218" TEXT="Erzeugen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1443740566042" ID="ID_632827470" MODIFIED="1443740575356" TEXT="Baum-Diff">
<node CREATED="1443740576744" FOLDED="true" ID="ID_335386387" MODIFIED="1488423342669" TEXT="Anwenden">
<icon BUILTIN="pencil"/>
<node CREATED="1443741563019" FOLDED="true" HGAP="-20" ID="ID_1944319966" MODIFIED="1488423342552" TEXT="Demo-Beispiel" VSHIFT="8">
<icon BUILTIN="button_ok"/>
<node CREATED="1443741578993" ID="ID_318844233" MODIFIED="1443741582069" TEXT="INIT"/>
<node CREATED="1443741591616" ID="ID_1102292452" MODIFIED="1443741607633" TEXT="INS + MUT _THIS_"/>
<node CREATED="1443741620052" ID="ID_1065499540" MODIFIED="1443741633054" TEXT="MOD"/>
<node CREATED="1443741736636" ID="ID_1110914687" MODIFIED="1443741738320" TEXT="PERM"/>
<node CREATED="1443741711120" ID="ID_1289424857" MODIFIED="1443741714651" TEXT="Rekursion"/>
</node>
<node CREATED="1443740607172" FOLDED="true" HGAP="21" ID="ID_1836453078" MODIFIED="1488423342552" TEXT="INIT" VSHIFT="14">
<node CREATED="1443740625649" ID="ID_1466659920" MODIFIED="1443740670333" TEXT="Attribute + Kinder"/>
<node CREATED="1443740621098" ID="ID_987166264" MODIFIED="1443740625134" TEXT="nur Kinder"/>
<node CREATED="1443740615699" ID="ID_93430092" MODIFIED="1443740679948" TEXT="Attribute"/>
</node>
<node CREATED="1443741317885" FOLDED="true" ID="ID_1933201882" MODIFIED="1488423342552" TEXT="NAV">
<node CREATED="1443741324236" FOLDED="true" ID="ID_1812765289" MODIFIED="1488423308120" TEXT="PICK">
<node CREATED="1443741332691" ID="ID_1059350985" MODIFIED="1443741335518" TEXT="Attribut"/>
<node CREATED="1443741337786" ID="ID_70415473" MODIFIED="1443741342997" TEXT="letztes Attribut"/>
<node CREATED="1443741507347" ID="ID_1882468426" MODIFIED="1443741513958" TEXT="nicht vorhandenes"/>
<node CREATED="1444349824563" ID="ID_572700596" MODIFIED="1444349836702" TEXT="anonym: Ref::CHILD"/>
</node>
<node CREATED="1443741349576" FOLDED="true" ID="ID_1257145184" MODIFIED="1488423308120" TEXT="AFTER">
<node CREATED="1443741365526" ID="ID_1412112386" MODIFIED="1443741372089" TEXT="Attribut"/>
<node CREATED="1443741373013" FOLDED="true" ID="ID_1922596569" MODIFIED="1488423308104" TEXT="letztes Attribut">
<node CREATED="1443741379924" ID="ID_1574470178" MODIFIED="1443741382960" TEXT="Attribut anf&#xfc;gen"/>
<node CREATED="1443741383579" ID="ID_69718214" MODIFIED="1443741387039" TEXT="Kind anf&#xfc;gen"/>
</node>
<node CREATED="1443741399145" ID="ID_625711902" MODIFIED="1443741409900" TEXT="Kind"/>
<node CREATED="1443741519689" ID="ID_1627922512" MODIFIED="1443741523076" TEXT="nicht vorhandenes"/>
<node CREATED="1443742264333" ID="ID_1331320583" MODIFIED="1443742268596" TEXT="_END_">
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1443741448683" ID="ID_1149106472" MODIFIED="1443741456637" TEXT="Frage: wrap erlauben">
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node CREATED="1443740818144" FOLDED="true" ID="ID_1606823522" MODIFIED="1488423342552" TEXT="INS">
<node CREATED="1443740822151" FOLDED="true" ID="ID_1509080074" MODIFIED="1488423308120" TEXT="Attribut">
<node CREATED="1443740849731" ID="ID_1736662710" MODIFIED="1443740851743" TEXT="vorne"/>
<node CREATED="1443740852219" ID="ID_546949723" MODIFIED="1443740853615" TEXT="hinten"/>
<node CREATED="1443740854491" ID="ID_730025656" MODIFIED="1443740858070" TEXT="Duplikat"/>
<node CREATED="1443740862570" ID="ID_899234602" MODIFIED="1443740866621" TEXT="Typ-Attribut"/>
<node CREATED="1443740918266" ID="ID_1583583367" MODIFIED="1443740923149" TEXT="Attribut in Kind-Zone"/>
</node>
<node CREATED="1443740870057" FOLDED="true" ID="ID_1029684307" MODIFIED="1488423308120" TEXT="Kind">
<node CREATED="1443740874416" ID="ID_1507242638" MODIFIED="1443740884674" TEXT="nach dem letzten Attribut"/>
<node CREATED="1443740885287" ID="ID_839165655" MODIFIED="1443740909455" TEXT="im Scope"/>
<node CREATED="1443740967099" ID="ID_1771095243" MODIFIED="1443740976438" TEXT="noch in der Attribut-Zone"/>
</node>
</node>
<node CREATED="1443740986217" FOLDED="true" ID="ID_1868954463" MODIFIED="1488423342552" TEXT="DEL">
<node CREATED="1443740988710" FOLDED="true" ID="ID_1745624753" MODIFIED="1488423308120" TEXT="Attribut">
<node CREATED="1443740997760" ID="ID_1984040083" MODIFIED="1443741004867" TEXT="normal"/>
<node CREATED="1443741005718" ID="ID_1595612872" MODIFIED="1443741014489" TEXT="in falscher Ordnung"/>
<node CREATED="1443741015221" ID="ID_587604738" MODIFIED="1443741030671" TEXT="Duplikat"/>
</node>
<node CREATED="1443741060111" FOLDED="true" ID="ID_1609626537" MODIFIED="1488423308120" TEXT="Kind">
<node CREATED="1443741062863" ID="ID_1087964841" MODIFIED="1443741072913" TEXT="normal"/>
<node CREATED="1443741074333" ID="ID_625561892" MODIFIED="1443741087863" TEXT="noch in der Attribut-Zone"/>
<node CREATED="1443741088619" ID="ID_290604972" MODIFIED="1443741095374" TEXT="in falscher Ordnung"/>
</node>
<node CREATED="1443741637530" ID="ID_397839136" MODIFIED="1443741640949" TEXT="Teilbaum"/>
</node>
<node CREATED="1443741106905" FOLDED="true" ID="ID_1001938821" MODIFIED="1488423342552" TEXT="PERM">
<node CREATED="1443741110392" ID="ID_513982563" MODIFIED="1443741112764" TEXT="Attribut"/>
<node CREATED="1443741113584" FOLDED="true" ID="ID_1631505651" MODIFIED="1488423308120" TEXT="Kind">
<node CREATED="1443741137141" ID="ID_300777204" MODIFIED="1443741140008" TEXT="normal"/>
<node CREATED="1443741140596" ID="ID_1663721251" MODIFIED="1443741147135" TEXT="mitten in Attribut-Zone"/>
<node CREATED="1443741147491" ID="ID_1992350599" MODIFIED="1443741152822" TEXT="am Ende der Attribut-Zone"/>
</node>
</node>
<node CREATED="1443741163385" FOLDED="true" ID="ID_1770431599" MODIFIED="1488423342552" TEXT="MUT">
<node CREATED="1443741167465" FOLDED="true" ID="ID_1585288084" MODIFIED="1488423308121" TEXT="Objekt by-name">
<node CREATED="1443741182559" ID="ID_1263613083" MODIFIED="1443741185922" TEXT="Attribut"/>
<node CREATED="1443741186486" ID="ID_1416713573" MODIFIED="1443741188010" TEXT="Kind"/>
<node CREATED="1443741654263" ID="ID_68796444" MODIFIED="1443741686295" TEXT="Teilbaum umordnen"/>
</node>
<node CREATED="1443741189590" FOLDED="true" ID="ID_407606511" MODIFIED="1488423308121" TEXT="Objekt _THIS_">
<node CREATED="1443741204516" ID="ID_899173442" MODIFIED="1443741207247" TEXT="nach INS"/>
<node CREATED="1443741212547" ID="ID_979817893" MODIFIED="1443741225989" TEXT="nach PICK"/>
<node CREATED="1443741226673" ID="ID_1242761745" MODIFIED="1443741229165" TEXT="nach AFTER"/>
<node CREATED="1443741229984" ID="ID_926730195" MODIFIED="1443741233636" TEXT="nach FIND"/>
</node>
</node>
<node CREATED="1460753406868" FOLDED="true" HGAP="-18" ID="ID_289459316" MODIFIED="1488423342552" TEXT="Spezialf&#xe4;lle" VSHIFT="25">
<node CREATED="1460753417178" FOLDED="true" ID="ID_1333254858" MODIFIED="1488423308121" TEXT="native bindings">
<linktarget COLOR="#639ad5" DESTINATION="ID_1333254858" ENDARROW="Default" ENDINCLINATION="1091;0;" ID="Arrow_ID_709688381" SOURCE="ID_534213210" STARTARROW="None" STARTINCLINATION="1150;185;"/>
<node CREATED="1460753433312" ID="ID_94456036" MODIFIED="1460753443667" TEXT="Binden an Collection von Primitiven"/>
<node CREATED="1460753444735" ID="ID_1100731476" MODIFIED="1460753459897" TEXT="Binden an Collection von intelligenten Spezialtypen"/>
<node CREATED="1460753460940" ID="ID_677690723" MODIFIED="1460753484941" TEXT="Komposit aus Attribut-Bindung und Kinder-Collection-Bindung"/>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1448063880238" ID="ID_159570161" MODIFIED="1448063892056" TEXT="Zyklus">
<node CREATED="1448063893293" ID="ID_1135705575" MODIFIED="1448063895640" TEXT="initial"/>
<node CREATED="1448063896300" ID="ID_1403759288" MODIFIED="1448063901895" TEXT="Zuordnung"/>
<node CREATED="1448063902387" ID="ID_1822852634" MODIFIED="1448063907367" TEXT="Aktion"/>
<node CREATED="1448063908243" ID="ID_474614648" MODIFIED="1448063912622" TEXT="Update"/>
<node CREATED="1448063913618" ID="ID_1967302094" MODIFIED="1448063944777" TEXT="Ende"/>
</node>
</node>
</node>
<node CREATED="1448314834155" ID="ID_1725514536" MODIFIED="1482524387497" POSITION="right" TEXT="Integral">
<node CREATED="1448315011484" ID="ID_867104544" MODIFIED="1482524639777" TEXT="generisch">
<node CREATED="1448315015107" ID="ID_1900900399" MODIFIED="1448315016511" TEXT="JSON">
<node CREATED="1448315031553" ID="ID_1177387568" MODIFIED="1448321655258" TEXT="parser">
<icon BUILTIN="button_ok"/>
<node CREATED="1448315067524" ID="ID_430746611" MODIFIED="1488423307320" TEXT="Libs">
<icon BUILTIN="info"/>
<node CREATED="1448315070908" FOLDED="true" ID="ID_1328571674" MODIFIED="1488423342670" TEXT="JSON-C">
<node CREATED="1448318526124" ID="ID_737325795" MODIFIED="1448318530232" TEXT="einfach"/>
<node CREATED="1448318531644" ID="ID_745699377" MODIFIED="1448318533543" TEXT="robust"/>
<node CREATED="1448318534203" ID="ID_1415146809" MODIFIED="1448318535863" TEXT="grottig"/>
</node>
<node CREATED="1448316910322" FOLDED="true" ID="ID_677006595" MODIFIED="1488423342670" TEXT="JsonCpp">
<node CREATED="1448318540363" ID="ID_269049852" MODIFIED="1448318568235" TEXT="sehr weit verbreitet"/>
<node CREATED="1448318569151" ID="ID_239373731" MODIFIED="1448318601522">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>schmerzloses </i>C++ API
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448318701621" ID="ID_1339095142" MODIFIED="1448318719135" TEXT="unterst&#xfc;tzt Builder f&#xfc;r Integration in den Parse-Vorgang"/>
<node CREATED="1448318726098" ID="ID_1386753436" MODIFIED="1448318736621" TEXT="unterst&#xfc;tzt Kommentarte (non-Standard)"/>
<node CREATED="1448318671009" ID="ID_1620559470" MODIFIED="1448318691459" TEXT="kann als eine gro&#xdf;e CPP-Datei eingebunden werden"/>
<node CREATED="1448319454226" ID="ID_108762123" MODIFIED="1448319476071">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Performance: guter Schnitt (etw. besser als boost spirit)
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448320352804" ID="ID_1738484079" MODIFIED="1448320354088" TEXT="MIT"/>
</node>
<node CREATED="1448316923520" FOLDED="true" ID="ID_1207744350" MODIFIED="1488423342670" TEXT="rapidjson">
<node CREATED="1448320356547" ID="ID_18433884" MODIFIED="1448320362967" TEXT="MIT / teils BSD"/>
<node CREATED="1448320418619" ID="ID_972456274" MODIFIED="1448320428198" TEXT="gilt als schnell und effizient"/>
<node CREATED="1448320428906" ID="ID_279600641" MODIFIED="1448321207817">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hat ein DOM-API <i>und</i>&#160;ein SAX-artiges API
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1448320473828" ID="ID_681519615" MODIFIED="1448320508422" TEXT="header-only, self-contained, keine Abh&#xe4;ngigkeiten!">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t: <b>nicht einmal</b>&#160;abh&#228;ngig von der STL
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448320555049" ID="ID_1836540415" MODIFIED="1448321597617" TEXT="erweiterter Unicode-Support, incl. recoding beim Parsen"/>
<node CREATED="1448321567452" ID="ID_933252727" MODIFIED="1448321582600" TEXT="kann auch in-situ parsen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wie gson
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448320545523" ID="ID_298176210" MODIFIED="1448320554597" TEXT="vollst&#xe4;ndig, incl Ersetzungen"/>
<node CREATED="1448321598848" ID="ID_1151875949" MODIFIED="1448321606499" TEXT="eigener Block-Allokator, ersetzbar"/>
</node>
<node CREATED="1448318965627" FOLDED="true" ID="ID_453761706" MODIFIED="1488423342670" TEXT="vjson / gason">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vjson war Google Code;
    </p>
    <p>
      nach dem Umzug auf Github hei&#223;t es gason
    </p>
  </body>
</html></richcontent>
<node CREATED="1448319925356" ID="ID_1589130421" MODIFIED="1448320242925" TEXT="angeblich ziemlich schnell">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      lt. eigenen Benchmakrs deutlich schneller als rapitjson, welches eigentlich immer als der &quot;schnelle&quot; JSON-Parser gilt.
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448319936915" ID="ID_1093677957" MODIFIED="1448319953316" TEXT="C++, aber ehr C-artiges API"/>
<node CREATED="1448319955328" ID="ID_534293846" MODIFIED="1448319962555" TEXT="type-tag, switch-on-type"/>
<node CREATED="1448319963711" ID="ID_1858072437" MODIFIED="1448320100768" TEXT="Parser ist destruktiv">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. das Parsen schreibt den Eingabepuffer um, und Strings bleiben einfach liegen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448319973830" ID="ID_1495739376" MODIFIED="1448320003901" TEXT="Werte werden in IEEE double NaN geboxt"/>
<node CREATED="1448320035173" ID="ID_226864203" MODIFIED="1448320053255" TEXT="eigener Block-Allokator"/>
<node CREATED="1448320365290" ID="ID_1044522194" MODIFIED="1448320366526" TEXT="MIT"/>
</node>
<node CREATED="1448315073819" FOLDED="true" ID="ID_280907535" MODIFIED="1488423342670" TEXT="boost spirit">
<node CREATED="1448316823037" ID="ID_726858418" MODIFIED="1448316825976" TEXT="reiner parser"/>
<node CREATED="1448321185310" ID="ID_180554028" MODIFIED="1448321192489" TEXT="kann direkt in Vector parsen"/>
<node CREATED="1448316826444" ID="ID_263014084" MODIFIED="1448316840095" TEXT="grammatik via metaprogrmming"/>
<node CREATED="1448316840474" ID="ID_1878535070" MODIFIED="1448321141123" TEXT="code bloat, vor allem in Debug-builds">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1448321146283" ID="ID_1257942632" MODIFIED="1448321166341" TEXT="Ver&#xf6;ffentlichung undurchsichtig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kein Repo auffindbar
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1448315077731" FOLDED="true" ID="ID_439331220" MODIFIED="1488423342670" TEXT="boost property-tree">
<node CREATED="1448316790553" ID="ID_1199102717" MODIFIED="1448316795764" TEXT="einfaches API"/>
<node CREATED="1448316796240" ID="ID_1402766358" MODIFIED="1448316803299" TEXT="parst in einen boost::property_tree"/>
<node CREATED="1448316803799" ID="ID_534763461" MODIFIED="1448321138090" TEXT="keine Typisierung">
<icon BUILTIN="smily_bad"/>
</node>
</node>
</node>
<node CREATED="1448320959060" ID="ID_937046843" MODIFIED="1448321217315" TEXT="Entscheidung">
<icon BUILTIN="yes"/>
<node CREATED="1448320963067" ID="ID_824752283" MODIFIED="1448321223059" TEXT="rapidjson">
<icon BUILTIN="info"/>
</node>
<node CREATED="1448320987776" FOLDED="true" ID="ID_545245790" MODIFIED="1488423342670" TEXT="Begr&#xfc;ndung...">
<node CREATED="1448321001518" ID="ID_671858484" MODIFIED="1448321049432">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ich will nicht <i>noch ein</i>&#160;Objekt-System
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1448321022444" ID="ID_658035448" MODIFIED="1448321032302" TEXT="das SAX-API ist genau, was ich brauche"/>
<node CREATED="1448321063078" ID="ID_923664235" MODIFIED="1448321070185" TEXT="au&#xdf;erdem klein und schmerzlos"/>
<node CREATED="1448321614174" ID="ID_1319618589" MODIFIED="1448321630999" TEXT="sch&#xf6;nes API, und ist schnell"/>
</node>
</node>
</node>
<node CREATED="1448315063541" ID="ID_430049414" MODIFIED="1448315065344" TEXT="framework">
<node CREATED="1448321310054" ID="ID_180974747" MODIFIED="1448321312681" TEXT="Forderungen">
<node CREATED="1448321258189" ID="ID_1374008180" MODIFIED="1448321323703" TEXT="diff::Record als Container"/>
<node CREATED="1448321329691" ID="ID_982254628" MODIFIED="1448321335478" TEXT="Validierung beim Parsen"/>
<node CREATED="1448321280178" ID="ID_1241470226" MODIFIED="1448321328887" TEXT="Erweiterbarkeit auf Objekt-Serialisieierung"/>
</node>
</node>
</node>
</node>
<node CREATED="1482524641484" ID="ID_1651495185" MODIFIED="1482524645893" TEXT="Architektur"/>
<node CREATED="1482524498822" ID="ID_431883229" MODIFIED="1482524501904" TEXT="Datenstrom"/>
<node CREATED="1482524516371" ID="ID_396707258" MODIFIED="1482524525561" TEXT="Event-Sourcing"/>
<node CREATED="1482524530842" ID="ID_606738640" MODIFIED="1482524535059" TEXT="Dependency-Injection"/>
<node CREATED="1482524535575" ID="ID_387248900" MODIFIED="1482524540392" TEXT="Extension-System"/>
<node CREATED="1482524569705" ID="ID_1815572475" MODIFIED="1482524583394" TEXT="Parametrisierung"/>
</node>
<node CREATED="1448314890907" ID="ID_411012156" MODIFIED="1488423307321" POSITION="right" TEXT="Session">
<icon BUILTIN="go"/>
<node CREATED="1481688464060" ID="ID_53574817" MODIFIED="1481688467351" TEXT="Architektur">
<node CREATED="1481688469507" ID="ID_1691953889" MODIFIED="1481688476974" TEXT="Session-Subsystem">
<node CREATED="1481688478057" ID="ID_1082882066" MODIFIED="1481688486685" TEXT="&#xe4;quivalent zum ProcDispatcher"/>
<node CREATED="1481688490000" ID="ID_579694361" MODIFIED="1481688499051" TEXT="koordiniert die Abl&#xe4;ufe in Proc"/>
</node>
<node CREATED="1481688500943" ID="ID_1821833408" MODIFIED="1482464848610" TEXT="Session ist eine interne Datenstruktur">
<icon BUILTIN="yes"/>
</node>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1484377665971" HGAP="28" ID="ID_641757454" MODIFIED="1484377682969" TEXT="Logik" VSHIFT="5">
<icon BUILTIN="hourglass"/>
</node>
<node CREATED="1481688517437" HGAP="24" ID="ID_241232196" MODIFIED="1483927980740" TEXT="Lifecycle" VSHIFT="8">
<icon BUILTIN="info"/>
<node CREATED="1481688521532" ID="ID_1825349679" MODIFIED="1483927971331" TEXT="Session startet on demand"/>
<node CREATED="1481688529539" ID="ID_1972961763" MODIFIED="1481688540109" TEXT="SessionManager ist zust&#xe4;ndig"/>
<node CREATED="1481688544921" ID="ID_1934560784" MODIFIED="1481688561386" TEXT="wenn Session geladen, ist ProcDispatcher freigegeben"/>
<node CREATED="1481688562830" ID="ID_708961458" MODIFIED="1481688582024" TEXT="SessionSubsystem startet processing loop im ProcDispatcher"/>
<node CREATED="1481777252497" ID="ID_1789585729" MODIFIED="1488423307323" TEXT="Shutdown">
<linktarget COLOR="#5a97df" DESTINATION="ID_1789585729" ENDARROW="Default" ENDINCLINATION="3216;0;" ID="Arrow_ID_1930701586" SOURCE="ID_1392452935" STARTARROW="None" STARTINCLINATION="8475;0;"/>
<node CREATED="1481777297635" ID="ID_483368010" MODIFIED="1481777305432" TEXT="Deadlock-Gefahr">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1481777308978" ID="ID_1380229085" MODIFIED="1481777327818" TEXT="Lebensdauer der Session-Datenstruktur">
<node CREATED="1481777328479" ID="ID_1668359018" MODIFIED="1481777336447" TEXT="unabh&#xe4;ngig vom Session-Subsystem">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1481777343548" ID="ID_354877994" MODIFIED="1481777350704" TEXT="irgendwann post main()"/>
</node>
<node CREATED="1481777355747" ID="ID_1804595708" MODIFIED="1481777364326" TEXT="Subsystem-&gt;end">
<node CREATED="1481777368330" ID="ID_1933237483" MODIFIED="1481777374277" TEXT="Halte-Nachricht"/>
<node COLOR="#338800" CREATED="1481777374801" ID="ID_1844558240" MODIFIED="1483654878632" TEXT="sicherstellen, da&#xdf; Loop anh&#xe4;lt">
<arrowlink COLOR="#f70841" DESTINATION="ID_1748096848" ENDARROW="Default" ENDINCLINATION="-353;-77;" ID="Arrow_ID_506637666" STARTARROW="None" STARTINCLINATION="-200;38;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1483654823454" ID="ID_397283548" MODIFIED="1483654831113" TEXT="shutdown -&gt; requireAction"/>
<node CREATED="1483654831773" ID="ID_911489519" MODIFIED="1483654845495" TEXT="shutdown-Trigger macht Notify"/>
<node CREATED="1483654846083" ID="ID_1926442698" MODIFIED="1483654852935" TEXT="Loop-Thread wacht auf"/>
<node CREATED="1483654853850" ID="ID_934617670" MODIFIED="1483654862885" TEXT="...und f&#xe4;llt aus der Loop heraus"/>
</node>
<node CREATED="1481777397542" ID="ID_1147135932" MODIFIED="1481778447806" TEXT="DispatcherLoop stirbt"/>
<node CREATED="1481777410972" ID="ID_457965735" MODIFIED="1481777418982" TEXT="sigTerm"/>
<node CREATED="1483927907096" ID="ID_1366384241" MODIFIED="1483927919263" TEXT="destruktor setzt auch PImpl auf NULL">
<icon BUILTIN="idea"/>
<node CREATED="1483927920718" ID="ID_409709261" MODIFIED="1483927929088" TEXT="niemand sonst fa&#xdf;t den PImpl an"/>
<node CREATED="1483927929773" ID="ID_1082393310" MODIFIED="1483927945790" TEXT="atomar wg. Lock auf ProcDispatcher"/>
</node>
</node>
</node>
</node>
<node CREATED="1481778538165" HGAP="37" ID="ID_522398894" MODIFIED="1488423307326" TEXT="ProcDispatcher" VSHIFT="12">
<icon BUILTIN="button_ok"/>
<node CREATED="1481826274018" HGAP="18" ID="ID_1916296972" MODIFIED="1482430840977" TEXT="Requirements" VSHIFT="-7">
<icon BUILTIN="button_ok"/>
<node CREATED="1481826329819" ID="ID_1238318698" MODIFIED="1481826431105" TEXT="enqueue commands concurrently">
<node CREATED="1481827297122" ID="ID_860095678" MODIFIED="1481827311268" TEXT="FIFO for regular commands"/>
<node CREATED="1481827312024" ID="ID_850193431" MODIFIED="1481827318187" TEXT="LIFO for priority requests">
<node CREATED="1482712100745" ID="ID_1387300775" MODIFIED="1482712106824" TEXT="vorerst nicht implementiert">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1481826431861" ID="ID_328583048" MODIFIED="1481826440935" TEXT="process sequentially"/>
<node CREATED="1481826441884" ID="ID_952327020" MODIFIED="1481826446791" TEXT="until queue is empty"/>
<node CREATED="1481826997930" ID="ID_576341056" MODIFIED="1488423307326" TEXT="provide a check point">
<node CREATED="1481827040748" ID="ID_406205627" MODIFIED="1481827045172" TEXT="when">
<icon BUILTIN="help"/>
<node CREATED="1481827019615" ID="ID_1169845292" MODIFIED="1481827024402" TEXT="after each command"/>
<node CREATED="1481827024942" ID="ID_137101226" MODIFIED="1483747035198" TEXT="after each builder run"/>
</node>
<node CREATED="1481827051787" ID="ID_1911969568" MODIFIED="1481827055340" TEXT="why">
<icon BUILTIN="help"/>
<node CREATED="1483747048432" ID="ID_1165337571" MODIFIED="1483747060594" TEXT="to ensure sane state logic"/>
<node CREATED="1483747065326" ID="ID_896973402" MODIFIED="1483747120443" TEXT="allow to wait for disabled state">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...necessary when closing the session;
    </p>
    <p>
      we need to wait for the current command or builder run to be completed
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1481827548073" ID="ID_690689259" MODIFIED="1488423307327" TEXT="manage entries">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...noch nicht implementiert 1/17
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="bell"/>
<node CREATED="1481827558336" ID="ID_1026688096" MODIFIED="1481827646856" TEXT="match current session"/>
<node CREATED="1481827647491" ID="ID_1849886325" MODIFIED="1481827652111" TEXT="aggregate similar"/>
<node CREATED="1481827652539" ID="ID_313823253" MODIFIED="1481827657846" TEXT="supersede"/>
</node>
</node>
<node CREATED="1481917572639" HGAP="74" ID="ID_190109346" MODIFIED="1488423307332" TEXT="operational..." VSHIFT="-27">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1481917612602" ID="ID_567971228" MODIFIED="1481917622388" TEXT="Loop l&#xe4;uft stets, aber blockt ggfs"/>
<node CREATED="1481917641910" ID="ID_1157396234" MODIFIED="1481917663639" TEXT="anf&#xe4;nglich idle --&gt; schlafen"/>
<node CREATED="1481918441020" ID="ID_584625018" MODIFIED="1481918463933" TEXT="Command-Queue wird ohne Verz&#xf6;gerung abgearbeitet"/>
<node CREATED="1481918465680" ID="ID_973127249" MODIFIED="1481918474355" TEXT="Builder startet">
<node CREATED="1481918475295" ID="ID_1997533333" MODIFIED="1481918480586" TEXT="nach kurzer Idle-Periode"/>
<node CREATED="1481918481102" ID="ID_335740959" MODIFIED="1481918491976" TEXT="nach l&#xe4;ngerer Command-Abarbeitung"/>
</node>
<node CREATED="1481917691047" ID="ID_714937554" MODIFIED="1481917700266" TEXT="&#xc4;nderung der Umst&#xe4;nde">
<node CREATED="1481917700998" ID="ID_470084004" MODIFIED="1481917705641" TEXT="kommt stets &#xfc;ber Interface"/>
<node CREATED="1481917726683" ID="ID_962119246" MODIFIED="1481917738197" TEXT="weckt den Loop-Thread auf"/>
<node CREATED="1481917766837" ID="ID_1794908304" MODIFIED="1481918114857" TEXT="ist atomar (locking)">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Guard beim Zugang &#252;ber das Interface
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1481918074269" ID="ID_1984339005" MODIFIED="1481918091136">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>nur sie</i>&#160;ist atomar
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1481918117831" ID="ID_924462902" MODIFIED="1483746949675" TEXT="asynchron">
<icon BUILTIN="info"/>
<node CREATED="1481918130245" ID="ID_1624017067" MODIFIED="1481918382409">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>nur ein Thread</b>&#160;f&#252;r Commands und Builder
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#171ccb" DESTINATION="ID_1214851922" ENDARROW="Default" ENDINCLINATION="53;-5;" ID="Arrow_ID_1745317422" STARTARROW="None" STARTINCLINATION="42;2;"/>
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1481918174199" ID="ID_364234189" MODIFIED="1481918180762" TEXT="essentiell f&#xfc;r Konsitenz der Session"/>
<node CREATED="1481918181254" ID="ID_1313994065" MODIFIED="1481918191233" TEXT="stellt bereits sicher, da&#xdf; der Builder blockt"/>
</node>
<node CREATED="1481918194356" ID="ID_1233041580" MODIFIED="1481918206823" TEXT="einf&#xfc;gen neuer Commands"/>
<node CREATED="1481918273490" ID="ID_1214851922" MODIFIED="1481918368739" TEXT="n&#xe4;chstes Command starten">
<arrowlink COLOR="#010de6" DESTINATION="ID_1857790160" ENDARROW="Default" ENDINCLINATION="86;0;" ID="Arrow_ID_1735750125" STARTARROW="Default" STARTINCLINATION="127;3;"/>
<linktarget COLOR="#171ccb" DESTINATION="ID_1214851922" ENDARROW="Default" ENDINCLINATION="53;-5;" ID="Arrow_ID_1745317422" SOURCE="ID_1624017067" STARTARROW="None" STARTINCLINATION="42;2;"/>
</node>
<node CREATED="1481918291351" ID="ID_1857790160" MODIFIED="1481918331776" TEXT="Builder-Lauf initiieren">
<linktarget COLOR="#010de6" DESTINATION="ID_1857790160" ENDARROW="Default" ENDINCLINATION="86;0;" ID="Arrow_ID_1735750125" SOURCE="ID_1214851922" STARTARROW="Default" STARTINCLINATION="127;3;"/>
</node>
<node CREATED="1481918212722" ID="ID_1376599707" MODIFIED="1481918221844" TEXT="Shutdown-Triggger"/>
<node CREATED="1481918222577" ID="ID_1588368762" MODIFIED="1481918245517" TEXT="Sperre setzen">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1481918517018" HGAP="11" ID="ID_916331172" MODIFIED="1483654611468" TEXT="Sperre" VSHIFT="8">
<icon BUILTIN="button_ok"/>
<node CREATED="1481918524673" ID="ID_1705856370" MODIFIED="1483654636642" TEXT="verhindert Command- und Builder-start">
<icon BUILTIN="info"/>
</node>
<node CREATED="1481918547182" ID="ID_920627724" MODIFIED="1483654595721" TEXT="bestehende Commands / Builder-L&#xe4;ufe werden noch abgeschlossen">
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1481920722100" ID="ID_1378897740" MODIFIED="1483654575032" TEXT="SessionManager mu&#xdf; auf Sperre warten">
<linktarget COLOR="#5a97df" DESTINATION="ID_1378897740" ENDARROW="Default" ENDINCLINATION="9540;0;" ID="Arrow_ID_1446486242" SOURCE="ID_284397306" STARTARROW="None" STARTINCLINATION="9540;0;"/>
<icon BUILTIN="button_ok"/>
</node>
<node COLOR="#338800" CREATED="1481926040310" ID="ID_667460598" MODIFIED="1483746929724" TEXT="Deadlock wenn der Session-Thread selber auf diese Sperre wartet">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1482197251133" ID="ID_1245878337" MODIFIED="1483746892436">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ticket <font color="#e90426">#1054</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1482197270202" ID="ID_221875120" MODIFIED="1482197288634" TEXT="neues API auf unserem Thread-Wrapper schaffen">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1482197280297" ID="ID_1142010083" MODIFIED="1482197291127" TEXT="rein technisch bereits heute m&#xf6;glich">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482197292263" ID="ID_669497196" MODIFIED="1482197456041" TEXT="jeder Lumiera-Thread speichert &quot;self&quot; in thread local storage">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und &quot;self&quot; == LumieraThrea* == &quot;handle&quot; (im Wrapper).
    </p>
    <p>
      
    </p>
    <p>
      D.h. solange der Wrapper lebt (!), kann er selber leicht feststellen, ob die aktuelle Ausf&#252;hrung
    </p>
    <p>
      auch in einem Thread stattfindet, der
    </p>
    <ul>
      <li>
        von unserem Threadpool gestartet wurde
      </li>
      <li>
        ein Thread-Handle hat, das mit dem Handle dieses Wrappers identisch ist.
      </li>
    </ul>
    <p>
      
    </p>
    <p>
      Das Sch&#246;ne bei diesem Ansatz ist, da&#223; man daf&#252;r weder das Handle exponieren mu&#223;,
    </p>
    <p>
      noch irgendwelche komischen Policies aufmachen. Solange es das Objekt gibt, klappt das.
    </p>
    <p>
      OO rocks!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
</node>
</node>
</node>
<node CREATED="1481828583551" HGAP="148" ID="ID_436508747" MODIFIED="1482430869706" TEXT="Aufbau" VSHIFT="-35">
<icon BUILTIN="button_ok"/>
<node CREATED="1481828611347" ID="ID_160907866" MODIFIED="1481828614071" TEXT="Front-End"/>
<node CREATED="1481828614827" ID="ID_1752707995" MODIFIED="1481828622958" TEXT="DispatcherLoop PImpl"/>
<node CREATED="1481828624042" ID="ID_1970372601" MODIFIED="1481828635748" TEXT="Looper f&#xfc;r Schleifen-Logik"/>
<node CREATED="1481828639584" ID="ID_1344182776" MODIFIED="1481828648402" TEXT="CommandQueue"/>
</node>
<node CREATED="1481778549940" HGAP="50" ID="ID_676848252" MODIFIED="1488423307337" TEXT="DispatcherLoop" VSHIFT="1">
<icon BUILTIN="button_ok"/>
<node CREATED="1481778741546" ID="ID_684867533" MODIFIED="1481778755694" TEXT="&#xf6;ffnet Session-Interface">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481778565298" ID="ID_1803510423" MODIFIED="1481778569181" TEXT="Schleifen-Logik">
<node CREATED="1481778570153" ID="ID_6904207" MODIFIED="1481778579395" TEXT="beruht auf CondVar / wait"/>
<node CREATED="1481831352239" ID="ID_947882756" MODIFIED="1481831357242" TEXT="Looper-Hilfskomponente"/>
<node CREATED="1481831372092" ID="ID_411828431" MODIFIED="1481832034056" TEXT="mu&#xdf;...">
<icon BUILTIN="yes"/>
<node CREATED="1481831398297" ID="ID_1100709659" MODIFIED="1481831407731" TEXT="aufwachen wenn Commands anstehen"/>
<node CREATED="1481831722406" ID="ID_462123533" MODIFIED="1481831730937" TEXT="aufwachen wenn Checkpoint"/>
<node CREATED="1481831408415" ID="ID_1091888781" MODIFIED="1481831424193" TEXT="aufwachen bei Shutdown"/>
<node CREATED="1481831432220" ID="ID_1256743195" MODIFIED="1481831446206" TEXT="Shutdown-Flag verwalten"/>
<node CREATED="1481831886448" FOLDED="true" ID="ID_1022163658" MODIFIED="1488423342670" TEXT="Betriebszustand">
<icon BUILTIN="info"/>
<node CREATED="1481831945472" ID="ID_756400184" MODIFIED="1481831947772" TEXT="arbeitend"/>
<node CREATED="1481831948240" ID="ID_349500680" MODIFIED="1481832004796" TEXT="Leerlauf"/>
<node CREATED="1481831950160" ID="ID_602481733" MODIFIED="1481831962578" TEXT="gesperrt"/>
<node CREATED="1481831965253" ID="ID_1644273707" MODIFIED="1481832013139" TEXT="sterbend"/>
</node>
<node CREATED="1481831638089" ID="ID_209292609" MODIFIED="1482375840758" TEXT="Builder steuern">
<arrowlink COLOR="#5242a4" DESTINATION="ID_1311208726" ENDARROW="Default" ENDINCLINATION="2;-45;" ID="Arrow_ID_313252247" STARTARROW="None" STARTINCLINATION="-186;-13;"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481831642376" FOLDED="true" ID="ID_1301681872" MODIFIED="1488423342670" TEXT="Checkpoint bieten">
<icon BUILTIN="button_ok"/>
<node CREATED="1482365008444" ID="ID_1520027430" MODIFIED="1482712182562" TEXT="warum">
<icon BUILTIN="help"/>
</node>
<node CREATED="1482365012947" ID="ID_10844185" MODIFIED="1482712196344" TEXT="um zuverl&#xe4;ssig anhalten zu k&#xf6;nnen"/>
<node CREATED="1482712199900" ID="ID_1990071363" MODIFIED="1482712291119" TEXT="Checkpoint == Zustand ausgeglichen">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482712212930" FOLDED="true" ID="ID_1065088226" MODIFIED="1488423342552" TEXT="das hei&#xdf;t">
<icon BUILTIN="info"/>
<node CREATED="1482712227048" ID="ID_172281578" MODIFIED="1482712234491" TEXT="keine schwebenden Zustands&#xe4;nderungen"/>
<node CREATED="1482712235016" ID="ID_943771142" MODIFIED="1482712249961" TEXT="Alle zustands-Relevanten &#xc4;nderungen sind ausgewertet"/>
<node CREATED="1482712250461" ID="ID_610930154" MODIFIED="1482712260744" TEXT="alle Pr&#xe4;dikate spiegeln den wirklichen Zustand wider"/>
<node CREATED="1482712261876" ID="ID_47966817" MODIFIED="1482712282765">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Kontrollflu&#223; ist <i>nicht</i>&#160;in einer Arbeitsfunktion
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
<node CREATED="1481831747042" ID="ID_134003952" MODIFIED="1481831788672" TEXT="sperrbar sein">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...und dann nur noch
    </p>
    <ul>
      <li>
        auf Shutdown reagieren
      </li>
      <li>
        mitbekommen wenn die Sperre aufgehoben wird
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1481778590790" ID="ID_1748096848" MODIFIED="1481826297245" TEXT="Anhalten">
<linktarget COLOR="#f70841" DESTINATION="ID_1748096848" ENDARROW="Default" ENDINCLINATION="-353;-77;" ID="Arrow_ID_506637666" SOURCE="ID_1844558240" STARTARROW="None" STARTINCLINATION="-200;38;"/>
<node CREATED="1481778666820" ID="ID_1860590145" MODIFIED="1481917471448" TEXT="Trigger-Variable im Looper">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1481917462421" ID="ID_200909201" MODIFIED="1481917474864" TEXT="shutdown-Trigger ebenda">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1481917478412" ID="ID_594556769" MODIFIED="1481917505131" TEXT="kombiniert enabled und shutdown">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1481778708207" HGAP="50" ID="ID_1311208726" MODIFIED="1488423307342" TEXT="Builder-Steuerung">
<linktarget COLOR="#5242a4" DESTINATION="ID_1311208726" ENDARROW="Default" ENDINCLINATION="2;-45;" ID="Arrow_ID_313252247" SOURCE="ID_209292609" STARTARROW="None" STARTINCLINATION="-186;-13;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1481828478741" ID="ID_1706868067" MODIFIED="1481828497838" TEXT="Hysterese"/>
<node CREATED="1481828498618" ID="ID_321199314" MODIFIED="1481828519539" TEXT="nach neuem Command, mit kleiner Verz&#xf6;gerung"/>
<node CREATED="1481828520176" ID="ID_239973202" MODIFIED="1481828532570" TEXT="bei voller Schlange mit Toleranzschwelle"/>
<node CREATED="1481828568129" ID="ID_295162940" MODIFIED="1481828574412" TEXT="Builder-Lauf ist monolithisch"/>
<node CREATED="1481928858318" ID="ID_1695005029" MODIFIED="1481928863793" TEXT="verdecken...">
<node CREATED="1481928864549" ID="ID_515765693" MODIFIED="1481928867281" TEXT="wenn gesperrt"/>
<node CREATED="1481928867893" ID="ID_1200076817" MODIFIED="1481928871448" TEXT="wenn in Shutdown"/>
</node>
<node CREATED="1481928874500" ID="ID_73724471" MODIFIED="1481928881567" TEXT="idle-timeout">
<node CREATED="1481928882723" ID="ID_770033134" MODIFIED="1482375680096" TEXT="dynamisch steuern">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481928888035" ID="ID_927205346" MODIFIED="1482375682035" TEXT="kurz, wenn der Builder laufen mu&#xdf;">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481928913687" ID="ID_696704157" MODIFIED="1482375684299" TEXT="lang sonst">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1481928939300" ID="ID_1625916543" MODIFIED="1482375686245" TEXT="kein Timeout wenn gesperrt">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node COLOR="#338800" CREATED="1481928958233" ID="ID_350657817" MODIFIED="1482375661113" TEXT="Builder-Lauf erzwingen">
<icon BUILTIN="button_ok"/>
<node CREATED="1481928966167" ID="ID_1511723472" MODIFIED="1481928974435" TEXT="wenn lange Timeout-Spanne &#xfc;berschritten"/>
<node CREATED="1481928978223" ID="ID_106761868" MODIFIED="1482375654382" TEXT="Implementierung erfordert Zeitmessung">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1482365133332" ID="ID_99275973" MODIFIED="1482365147008" TEXT="moderne / zeitgem&#xe4;&#xdf;e Implementierung">
<icon BUILTIN="help"/>
</node>
<node CREATED="1482365613228" ID="ID_1195899391" MODIFIED="1488423307343">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>billig:</i>&#160;unsere Zeit-Lib nutzen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1482430715396" ID="ID_1005651794" MODIFIED="1482430722896" TEXT="nicht besonders sch&#xf6;n">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1482430724019" ID="ID_880234680" MODIFIED="1482430733678" TEXT="sollte mit std::chrono integrieren">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1482202066204" HGAP="23" ID="ID_41541843" MODIFIED="1482202078184" TEXT="L&#xf6;sungnsansatz" VSHIFT="16">
<icon BUILTIN="help"/>
<node CREATED="1482202080675" ID="ID_12774028" MODIFIED="1482202126233" TEXT="CommandQueue mu&#xdf; helfen und dirty-state verwalten">
<icon BUILTIN="button_cancel"/>
<node CREATED="1482202099752" ID="ID_434677221" MODIFIED="1482202114130" TEXT="geht, weil sie wei&#xdf;, wann ein Command wirklich l&#xe4;uft"/>
<node CREATED="1482202114630" ID="ID_1833945556" MODIFIED="1482202123872" TEXT="unsch&#xf6;n, weil wir nun massive Kopplung haben"/>
</node>
<node CREATED="1482202127252" ID="ID_1720469294" MODIFIED="1482375697342" TEXT="Looper in State-machine verwandeln">
<icon BUILTIN="button_ok"/>
<node CREATED="1482202136235" ID="ID_239151488" MODIFIED="1482202175455" TEXT="schwierig">
<icon BUILTIN="stop-sign"/>
</node>
<node CREATED="1482202142818" ID="ID_1412852448" MODIFIED="1482202181794" TEXT="ben&#xf6;tigt Hilfe von der DispatcherLoop">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1482202156416" ID="ID_1161547825" MODIFIED="1482202183920" TEXT="geht aber: nach jedem WHILE-Durchlauf">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482202165423" ID="ID_1845799711" MODIFIED="1482202186673" TEXT="schaut sauberer aus">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1482202187620" FOLDED="true" ID="ID_259250017" MODIFIED="1488423342671" TEXT="geht das &#xfc;berhaupt">
<icon BUILTIN="help"/>
<node CREATED="1482375716922" ID="ID_1702615812" MODIFIED="1482375756924" TEXT="ja">
<icon BUILTIN="forward"/>
</node>
<node CREATED="1482375724321" ID="ID_218202011" MODIFIED="1482375736264" TEXT="einfacher trap-door-Mechanismus">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1482375737327" ID="ID_1980564250" MODIFIED="1482375746202" TEXT="tendentiell bereits schwer zu verstehen">
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
</node>
<node COLOR="#338800" CREATED="1482202207242" ID="ID_1287224856" MODIFIED="1482375808494" TEXT="Test">
<icon BUILTIN="button_ok"/>
<node CREATED="1482202211049" ID="ID_1159832087" MODIFIED="1482202222520" TEXT="komplexen Ablauf als Szenario durchspielen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1482375826635" ID="ID_1275445978" MODIFIED="1482375832687" TEXT="beinhaltet auch einen Timeout">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1482202223536" ID="ID_374030732" MODIFIED="1482375796750">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Logik im Looper auf Basis
    </p>
    <p>
      generischer &#220;berlegungen implementiert
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1482202243253" ID="ID_1342399342" MODIFIED="1482202264229" TEXT="verh&#xe4;lt er sich dann richtig">
<icon BUILTIN="help"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1448314932726" ID="ID_669869188" MODIFIED="1484377639206" POSITION="right" TEXT="Render">
<icon BUILTIN="stop"/>
</node>
<node BACKGROUND_COLOR="#c9d1da" COLOR="#2d2198" CREATED="1439664045448" HGAP="240" ID="ID_21531707" MODIFIED="1439664212589" POSITION="left" TEXT="Info" VSHIFT="-500">
<edge COLOR="#b4a9e3"/>
<font NAME="SansSerif" SIZE="16"/>
<node CREATED="1439664217489" ID="ID_104059794" MODIFIED="1439664227516" TEXT="GTK-3">
<node CREATED="1439664230168" ID="ID_235548644" LINK="https://wiki.gnome.org/Projects/GTK%2B/Inspector" MODIFIED="1488423307344" TEXT="GKT+ Inspector">
<icon BUILTIN="idea"/>
<node CREATED="1439664318604" ID="ID_1327496126" MODIFIED="1439664322871" TEXT="keyboard shortcut">
<node CREATED="1439664330354" ID="ID_257893375" MODIFIED="1439664338622" TEXT="SHIFT-Ctrl I"/>
<node CREATED="1439664339353" ID="ID_559550607" MODIFIED="1439664367761" TEXT="aktivieren via dconf">
<node CREATED="1439664358287" ID="ID_1044303564" MODIFIED="1439664384559" TEXT="apt-get install dconf-editor"/>
<node CREATED="1439664574202" ID="ID_1254903463" MODIFIED="1439664576765" TEXT="org &gt; gtk &gt; settings &gt; debug"/>
</node>
</node>
<node CREATED="1439664638857" ID="ID_1095180651" MODIFIED="1439664652889">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>saugeil</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1477523681978" HGAP="23" ID="ID_400257229" MODIFIED="1477523760062" TEXT="Richtlinien" VSHIFT="-21">
<node CREATED="1477523701544" ID="ID_1802991794" MODIFIED="1477523708299" TEXT="empfohlen">
<node CREATED="1477523709342" ID="ID_781118281" MODIFIED="1477523710386" TEXT="Box">
<node CREATED="1477524273619" ID="ID_216152004" MODIFIED="1477524281302" TEXT="pack_start() / pack_end()"/>
<node COLOR="#ca1b00" CREATED="1477524282122" ID="ID_1542686719" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/sec-multi-item-containers.html.en#per-child-packing-options" MODIFIED="1477524315404" TEXT="packing options">
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1477523710854" ID="ID_1520577996" MODIFIED="1477523711882" TEXT="Grid"/>
</node>
<node CREATED="1477523714526" ID="ID_654535915" MODIFIED="1487275491415" TEXT="Application verwenden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...nicht mehr das <i>klassische</i>&#160;gtk::Main
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1477785704584" ID="ID_528329491" MODIFIED="1477785711408" TEXT="event handling">
<node CREATED="1477785727469" ID="ID_129787833" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/chapter-signals.html.en" MODIFIED="1477785743493" TEXT="Guide: Kapitel &quot;Signals&quot;"/>
<node CREATED="1477785747138" ID="ID_4618201" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/sec-xeventsignals.html.en" MODIFIED="1477785785076" TEXT="speziell Details zu event Handlern: X event signals">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1477785807338" ID="ID_80062368" MODIFIED="1477788924929" TEXT="wie">
<icon BUILTIN="info"/>
<node CREATED="1477785809930" ID="ID_596393977" MODIFIED="1477785817884" TEXT="signal connect()"/>
<node COLOR="#ca1b00" CREATED="1477785830295" ID="ID_94782325" MODIFIED="1477785836644" TEXT="-- oder --"/>
<node CREATED="1477785818920" ID="ID_1600280983" MODIFIED="1477788924929" TEXT="on_xxx() &#xfc;berschreiben">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_1600280983" ENDARROW="Default" ENDINCLINATION="893;0;" ID="Arrow_ID_1707544457" SOURCE="ID_111966354" STARTARROW="None" STARTINCLINATION="893;0;"/>
</node>
</node>
<node CREATED="1477785856731" ID="ID_63204089" MODIFIED="1477785865998" TEXT="R&#xfc;ckgabewert: true == fertig behandelt"/>
</node>
<node CREATED="1477526858307" ID="ID_676269117" MODIFIED="1477526862502" TEXT="custom widget">
<node COLOR="#ca1b00" CREATED="1477526864162" ID="ID_1385929770" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/sec-custom-widgets.html.en" MODIFIED="1477788908756" TEXT="Beispiel">
<linktarget COLOR="#a9b4c1" DESTINATION="ID_1385929770" ENDARROW="Default" ENDINCLINATION="1055;0;" ID="Arrow_ID_1314305180" SOURCE="ID_426069181" STARTARROW="None" STARTINCLINATION="1055;0;"/>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1477527104426" ID="ID_334736261" MODIFIED="1487275491447" TEXT="custom style properties">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...k&#246;nnen vom CSS-Stylesheet aus gesetzt werden.
    </p>
    <p>
      Siehe Beschreibung im Beispiel/Tutorial
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
</node>
<node CREATED="1446515847047" ID="ID_290915762" MODIFIED="1488423307345" TEXT="GtkCssProvider">
<node CREATED="1446515865029" ID="ID_1503616150" MODIFIED="1446515869624" TEXT="parsing errors"/>
<node CREATED="1477784584821" ID="ID_1810760662" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/sec-custom-widgets.html.en" MODIFIED="1477784889994" TEXT="Beispiel im GTKmm-Guide (custom widget)">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      in der Implementierung, mywidget.cc
    </p>
    <p>
      ist eine komplette Sequenz, wie man einen CSS-StyleProvider setzt
    </p>
    <p>
      und auch ein Signal f&#252;r Parse-Fehler anschlie&#223;t
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#a9b4c1" DESTINATION="ID_1810760662" ENDARROW="Default" ENDINCLINATION="1846;-61;" ID="Arrow_ID_1610122569" SOURCE="ID_1164942946" STARTARROW="None" STARTINCLINATION="-227;-646;"/>
<icon BUILTIN="info"/>
</node>
</node>
<node CREATED="1476375676818" ID="ID_1029920942" MODIFIED="1476375683685" TEXT="Canvas">
<node CREATED="1476375696215" ID="ID_1397403295" MODIFIED="1476375700698" TEXT="GtkDrawingArea">
<node CREATED="1476375709157" ID="ID_981405457" MODIFIED="1476375712568" TEXT="custom drawing"/>
</node>
<node CREATED="1476375702166" ID="ID_290806424" MODIFIED="1476375741250" TEXT="GtkLayout">
<icon BUILTIN="idea"/>
<node CREATED="1476375715644" ID="ID_1233704735" MODIFIED="1476375723495" TEXT="scrollable"/>
<node CREATED="1476375723915" ID="ID_72518723" MODIFIED="1476375732734" TEXT="can place sub-widgets"/>
<node CREATED="1476375733298" ID="ID_1024321268" MODIFIED="1476375738517" TEXT="allows custom drawing"/>
<node CREATED="1476468254712" HGAP="64" ID="ID_1424353399" MODIFIED="1476468264189" TEXT="Eigenschaften" VSHIFT="45">
<icon BUILTIN="pencil"/>
<node CREATED="1476468265399" FOLDED="true" HGAP="2" ID="ID_257790579" MODIFIED="1488423342671" TEXT="Test-Setup schaffen" VSHIFT="-15">
<node CREATED="1476468390830" FOLDED="true" ID="ID_1922633406" MODIFIED="1488423342552" TEXT="soll">
<icon BUILTIN="yes"/>
<node CREATED="1476468393174" ID="ID_412857359" MODIFIED="1476468395498" TEXT="schnell gehen"/>
<node CREATED="1476468395941" ID="ID_1419405883" MODIFIED="1476468401560" TEXT="nicht das UI kaputtmachen"/>
<node CREATED="1476468402085" ID="ID_1171941848" MODIFIED="1476468410471" TEXT="realistisch sein"/>
</node>
</node>
<node CREATED="1476468272510" FOLDED="true" ID="ID_150722005" MODIFIED="1488423342671" TEXT="Untersuchung">
<node CREATED="1477595404814" FOLDED="true" HGAP="37" ID="ID_1078486680" MODIFIED="1488423342553" TEXT="Goals" VSHIFT="-12">
<icon BUILTIN="yes"/>
<node CREATED="1476468292804" ID="ID_283291482" MODIFIED="1476468314044" TEXT="overlapping widgets"/>
<node CREATED="1476468292805" ID="ID_140846763" MODIFIED="1476468325987" TEXT="resizing of widgets"/>
<node CREATED="1476468292805" ID="ID_1075320324" MODIFIED="1487275491503" TEXT="partially covered widgets">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....how does the event dispatching deal with partially covered widgets
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1476468292805" ID="ID_1670607748" MODIFIED="1487275491510" TEXT="tabbing / focus order">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...for embedded widgets
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1476468292805" ID="ID_472068538" MODIFIED="1476468383131" TEXT="custom drawing and widget drawing"/>
</node>
<node CREATED="1477595408918" FOLDED="true" ID="ID_387371672" MODIFIED="1488423342553" TEXT="Tasks">
<node CREATED="1477595418669" FOLDED="true" ID="ID_1316314985" MODIFIED="1488423308121" TEXT="# place some simple widgets (Buttons)">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-1"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1477601706168" ID="ID_130395672" MODIFIED="1477601711906" TEXT="Kind-Klasse anlegen"/>
<node CREATED="1477601712063" ID="ID_1720776876" MODIFIED="1477601717354" TEXT="Kinder managen"/>
</node>
<node CREATED="1477595418669" FOLDED="true" ID="ID_517122292" MODIFIED="1488423308121" TEXT="# learn how to draw">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-2"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1477788816168" FOLDED="true" ID="ID_904860978" MODIFIED="1488423308105" TEXT="documentation">
<icon BUILTIN="info"/>
<node CREATED="1477788823751" ID="ID_111966354" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/chapter-drawingarea.html.en" MODIFIED="1477788918987" TEXT="read the custom drawing chapter">
<arrowlink DESTINATION="ID_1600280983" ENDARROW="Default" ENDINCLINATION="893;0;" ID="Arrow_ID_1707544457" STARTARROW="None" STARTINCLINATION="893;0;"/>
</node>
<node CREATED="1477788855234" ID="ID_426069181" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/sec-custom-widgets.html.en" MODIFIED="1477788908756" TEXT="read the example code of a custom widget">
<arrowlink DESTINATION="ID_1385929770" ENDARROW="Default" ENDINCLINATION="1055;0;" ID="Arrow_ID_1314305180" STARTARROW="None" STARTINCLINATION="1055;0;"/>
</node>
<node CREATED="1477789514451" ID="ID_84890591" LINK="https://www.cairographics.org/documentation/cairomm/reference/" MODIFIED="1477789523593" TEXT="Cairomm API-doc"/>
</node>
<node CREATED="1477784938038" FOLDED="true" ID="ID_93104177" MODIFIED="1488423308105" TEXT="how-to...">
<icon BUILTIN="button_ok"/>
<node CREATED="1477788718965" ID="ID_1354942176" MODIFIED="1477788722040" TEXT="derived class"/>
<node CREATED="1477788722228" ID="ID_720985572" MODIFIED="1477788730239" TEXT="override on_draw()"/>
<node CREATED="1477788731163" ID="ID_421749777" MODIFIED="1477788740646" TEXT="invoke inherited on_draw()">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1477788749289" ID="ID_1466181075" MODIFIED="1487275491561" TEXT="returns false">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...meaning, &quot;this event is not yet fully processed&quot;,
    </p>
    <p>
      i.e. the enclosing parent widget also gets a chance to redraw itself
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1477791881630" FOLDED="true" HGAP="37" ID="ID_1823256674" MODIFIED="1488423308105" TEXT="coordinates" VSHIFT="11">
<icon BUILTIN="button_ok"/>
<node CREATED="1477791885614" ID="ID_380369648" MODIFIED="1477791916343">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#bd0053">Warning</font>: allocation is the <i>visible</i>&#160;area
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1477791921953" ID="ID_555455740" MODIFIED="1477791952896" TEXT="...not the extension of the canvas!"/>
<node CREATED="1477841655353" FOLDED="true" ID="ID_1533125137" MODIFIED="1488423308090" TEXT="adjust to compensate">
<icon BUILTIN="idea"/>
<node CREATED="1477841600713" ID="ID_1319450558" LINK="http://stackoverflow.com/questions/40325668/scrollable-drawing-in-gtklayout" MODIFIED="1477841645668" TEXT="Question: can the framework adjust for us">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      asked on stackoverflow...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1477841687997" FOLDED="true" ID="ID_1267996486" MODIFIED="1488423308070" TEXT="explicitly by code">
<node CREATED="1477841706107" ID="ID_1685967940" MODIFIED="1477841722860" TEXT="use get_value() from adjustment"/>
<node CREATED="1477841728496" ID="ID_935017463" MODIFIED="1477841747713" TEXT="because this represents coord. of visible viewport"/>
<node CREATED="1477841753180" ID="ID_501431504" MODIFIED="1477841795159" TEXT="max(value) + viewport-size == canvas size">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...as can be observed
    </p>
    <p>
      by printing values from the on_draw() callback
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1477841809741" FOLDED="true" ID="ID_1400891730" MODIFIED="1488423307350" TEXT="context-&gt;translate">
<node CREATED="1477841816516" ID="ID_642580279" MODIFIED="1477841841348" TEXT="by -value"/>
<node CREATED="1477841842512" ID="ID_1549119493" MODIFIED="1477841854051" TEXT="allows us to use absolute coords."/>
<node CREATED="1477841854807" ID="ID_1202368941" MODIFIED="1477841859362" TEXT="clipping happens automatically"/>
<node CREATED="1477841860278" ID="ID_208350074" MODIFIED="1477841903458" TEXT="need to ctx-&gt;save() and ctx-&gt;restore()">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...otherwise adjustment values will cummulate,
    </p>
    <p>
      causing us to adjust too much
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1477843534095" FOLDED="true" ID="ID_1261380784" MODIFIED="1488423308090" TEXT="Problem: widget extension">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1477843543126" ID="ID_1096450231" MODIFIED="1477843556632" TEXT="seems to be defined only after drawing it"/>
<node CREATED="1478032584364" ID="ID_1961636905" MODIFIED="1478032597162" TEXT="need to find the right signal">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1478032598165" FOLDED="true" ID="ID_275509060" MODIFIED="1488423308070" TEXT="need to get foreach working">
<icon BUILTIN="yes"/>
<node CREATED="1478032633704" ID="ID_1067159761" MODIFIED="1478032641714" TEXT="Hint: show_all_children"/>
<node CREATED="1478032642310" ID="ID_246127915" LINK="https://developer.gnome.org/gtkmm-tutorial/stable/sec-custom-containers.html.en" MODIFIED="1478032658384" TEXT="Tutorial: custom container"/>
<node CREATED="1478037794935" FOLDED="true" HGAP="41" ID="ID_614099643" MODIFIED="1488423307350" TEXT="geht doch" VSHIFT="38">
<icon BUILTIN="ksmiletris"/>
<node CREATED="1478037802776" ID="ID_1706111079" MODIFIED="1478037817305" TEXT="keine Ahnung warums neulich einen SEGFAULT gab"/>
<node CREATED="1478037818037" ID="ID_1257597772" MODIFIED="1478037846037" TEXT="wichtig: Funktor inline bestehen lassen"/>
<node CREATED="1478037849449" ID="ID_171841689" MODIFIED="1478037859121" TEXT="habe diesmal sogar ein Lambda verwendet">
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1478037888539" FOLDED="true" ID="ID_1261247197" MODIFIED="1488423308070" TEXT="wie">
<icon BUILTIN="idea"/>
<node CREATED="1478037893658" ID="ID_1711061664" MODIFIED="1478037896669" TEXT="trigger-Variable"/>
<node CREATED="1478037899458" FOLDED="true" ID="ID_132096735" MODIFIED="1488423307350" TEXT="im on_draw()-Callback">
<node CREATED="1478037908465" ID="ID_1414994452" MODIFIED="1478037915835" TEXT="erst hier sind die Allokationen gesetzt"/>
<node CREATED="1478037916303" ID="ID_1781865207" MODIFIED="1478037977495" TEXT="konnte keine anderen sinnvollen callbacks finden">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...die anderen, die noch in Frage kommen w&#252;rden,
    </p>
    <p>
      sind nur f&#252;r den Fall, da&#223; ein Widget neu instantiiert wird
    </p>
    <p>
      oder neu in das Window-System gemappt wird.
    </p>
    <p>
      
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1478037986046" ID="ID_1293899679" MODIFIED="1478037990857">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      on_check_resize() wird nicht aufgerufen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
</node>
<node CREATED="1478038003892" ID="ID_477362495" MODIFIED="1478038010982" TEXT="rufe foreach mit inline-Lambda auf"/>
<node CREATED="1478038011427" ID="ID_399044896" MODIFIED="1478038019797" TEXT="melde per max in die lokalen Variablen zur&#xfc;ck"/>
<node CREATED="1478038020338" ID="ID_541293489" MODIFIED="1478038026524" TEXT="Layout-&gt;set_size()"/>
</node>
</node>
</node>
<node CREATED="1477784943797" FOLDED="true" ID="ID_298493039" MODIFIED="1488423308105" TEXT="layering">
<icon BUILTIN="button_ok"/>
<node CREATED="1477791842091" ID="ID_671463871" MODIFIED="1477791848942" TEXT="depends on order of parent call"/>
<node CREATED="1477791849578" ID="ID_96793101" MODIFIED="1477791858929" TEXT="and on order of cairo draw operations">
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1477784949533" ID="ID_1496795315" MODIFIED="1477784952776" TEXT="custom styling"/>
</node>
<node CREATED="1477595418670" ID="ID_770195423" MODIFIED="1477596119220" TEXT="# place a huge number of widgets, to scrutinise scrolling and performance">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-3"/>
</node>
<node CREATED="1477595418670" FOLDED="true" ID="ID_423514526" MODIFIED="1488423308121" TEXT="# place widgets overlapping / irregularily">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-4"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1477668663507" ID="ID_1576657272" MODIFIED="1477668671481" TEXT="order: later on top">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1477668674577" ID="ID_431152757" MODIFIED="1477694925233" TEXT="place widgets out of sight">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1477694911349" ID="ID_93004070" MODIFIED="1477694932280" TEXT="widgets beyond the scrollable area">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1477595418670" ID="ID_1370967982" MODIFIED="1478039086087" TEXT="# bind signals to those widgets, to verify event dispatching">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-5"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1477595418670" ID="ID_1603374862" MODIFIED="1477596119219" TEXT="# bind some further signal(s) to the GtkLayout container">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-6"/>
</node>
<node CREATED="1477595418670" ID="ID_496085779" MODIFIED="1478039095462" TEXT="# hide and re-show a partially and a totally overlapped widget">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-7"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1477595418670" FOLDED="true" ID="ID_1516591439" MODIFIED="1488423308121" TEXT="# move and delete widgets">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-8"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1477703725690" FOLDED="true" ID="ID_554135449" MODIFIED="1488423308105" TEXT="move">
<icon BUILTIN="button_ok"/>
<node CREATED="1477703730073" ID="ID_1070867956" MODIFIED="1477703771801" TEXT="foreach doesn&apos;t work">
<icon BUILTIN="smiley-angry"/>
</node>
<node CREATED="1479424603959" ID="ID_194428894" MODIFIED="1487275491645" TEXT="works on second attempt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...keine Ahnung, was ich beim ersten Mal falsch gemacht habe.
    </p>
    <p>
      jedenfalls hab ich da sofort beim ersten Aufruf der Closure einen SEGFAULT bekommen.
    </p>
    <p>
      
    </p>
    <p>
      Auch im zweiten Anlauf habe ich ein Lambda verwendet.
    </p>
    <p>
      M&#246;glicherweise ist der einzige Unterschied, da&#223; ich es nun aus dem draw-callback
    </p>
    <p>
      aufrufe, und da&#223; demgegen&#252;ber bei der ersten Verwendung die Allocation des jeweiligen
    </p>
    <p>
      Kind-Widgets noch gar nicht festgelegt war (denn das passiert erst beim draw).
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1477703741232" ID="ID_942790225" MODIFIED="1477703768906" TEXT="get_children returns copy">
<icon BUILTIN="smiley-oh"/>
</node>
</node>
<node CREATED="1477748350140" ID="ID_1672717051" MODIFIED="1477748361823" TEXT="align all in a row">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1477749635577" ID="ID_592134506" MODIFIED="1477749644284" TEXT="delete arbitrary children">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1477595418670" ID="ID_1275908576" MODIFIED="1477757749992" TEXT="# expand an existing widget (text change)">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-9"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1477595418670" ID="ID_700977265" MODIFIED="1477596119218" TEXT="# build a custom &quot;&apos;&apos;clip&apos;&apos;&quot; widget">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-1"/>
<icon BUILTIN="full-0"/>
</node>
<node CREATED="1477595418670" ID="ID_1708640864" MODIFIED="1477596119217" TEXT="# retrofit all preceding tests to use this &quot;&apos;&apos;clip&apos;&apos;&quot; widget">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-1"/>
<icon BUILTIN="full-1"/>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1482365415326" HGAP="87" ID="ID_886002365" MODIFIED="1482365429434" TEXT="Standard C++" VSHIFT="-21">
<node CREATED="1482365430484" ID="ID_1414724077" MODIFIED="1482365432879" TEXT="chrono">
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1482365434203" ID="ID_1026694670" MODIFIED="1482365437852" TEXT="lernen">
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1482365448257" ID="ID_1648116305" MODIFIED="1482365468841">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ticket <font color="#ba0c3f">#886</font>
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1450488895106" ID="ID_1586185818" MODIFIED="1488423307351" TEXT="Threading">
<node CREATED="1450488902049" ID="ID_1126260262" MODIFIED="1450488906100" TEXT="static init">
<node CREATED="1450489361187" ID="ID_1013681859" LINK="http://stackoverflow.com/a/17803616/444796" MODIFIED="1450489380816" TEXT="gute Zusammenfassung (SO)">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1450489388912" ID="ID_1935538293" MODIFIED="1450489435398" TEXT="function-static">
<icon BUILTIN="info"/>
<node CREATED="1450489395711" ID="ID_1661542298" MODIFIED="1450489410592" TEXT="when ctrl flow enters first"/>
<node CREATED="1450489412364" ID="ID_965405072" LINK="http://stackoverflow.com/a/8102145/444796" MODIFIED="1450489553742" TEXT="threadsafe since C++11"/>
<node CREATED="1450489421795" ID="ID_12247357" MODIFIED="1450489566108" TEXT="lives until the end of the program"/>
</node>
</node>
</node>
<node CREATED="1491098340549" ID="ID_938211341" MODIFIED="1491098343944" TEXT="Toolchain">
<node CREATED="1491098346053" ID="ID_1333373693" MODIFIED="1491098349439" TEXT="Doxygen">
<node CREATED="1491098370281" ID="ID_1904535196" MODIFIED="1491098373325" TEXT="Konfig"/>
<node CREATED="1491098373816" ID="ID_1022990703" MODIFIED="1491098376332" TEXT="Fallen">
<node CREATED="1491099064125" ID="ID_651937703" MODIFIED="1491099078088" TEXT="Dateien ohne @file werden nicht dokumentiert"/>
<node CREATED="1491099088362" ID="ID_1507484869" MODIFIED="1491109777999" TEXT="Template-Parameter in Klassennamen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn eine getemplatete Klasse zum Qualifizieren eines Feldes verwendet wird,
    </p>
    <p>
      dann m&#252;ssen die formalen Template-Parameter in spitzen Klammern mit angegeben werden.
    </p>
    <p>
      D.h. Doxygen ist hier genauso penibel wie C++ selber
    </p>
    <p>
      
    </p>
    <p>
      Beispiel
    </p>
    <p>
      Query&lt;RES&gt;::resolveBy
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1491099311253" ID="ID_307124479" MODIFIED="1491099409234">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      @param hat <b>stets</b>&#160;einen Parameternamen als Argument
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...der ist <i>nicht</i>&#160;optional
    </p>
    <p>
      vielmehr wird blindlings immer das erste Wort genommen.
    </p>
    <p>
      Wenn der Parameter selber nicht benannt ist (z.B. pure virtual function),
    </p>
    <p>
      kann man ersatzweise einfach einen Typnamen angeben.
    </p>
    <p>
      Sofern alle Parameter dokumentiert sind, klappt das.
    </p>
    <p>
      sonst kommt Doxygen durcheinander
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1491098382888" ID="ID_1568153886" MODIFIED="1491098384859" TEXT="Probleme">
<node CREATED="1491098385823" ID="ID_801230289" MODIFIED="1491098392530" TEXT="Auto-Links">
<node CREATED="1491098393405" ID="ID_1408589913" MODIFIED="1491098400681" TEXT="funktionieren in vielen F&#xe4;llen nicht"/>
<node CREATED="1491098402197" ID="ID_1572559975" MODIFIED="1491098411983" TEXT="Verdacht: mu&#xdf; Namespaces qualifizieren?"/>
<node CREATED="1491098427785" ID="ID_719835617" MODIFIED="1491098484327" TEXT="Beispiel: alle Unit-Tests">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....haben in ihrem @file-Kommentar
    </p>
    <p>
      einen Verweis \ref DieserUnit_test
    </p>
    <p>
      Und obwohl das der exakte Klassennahme ist,
    </p>
    <p>
      und obwohl genau diese Klasse im Klassenindex zu finden ist
    </p>
    <p>
      wird hier kein Link erzeugt
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1491098487889" ID="ID_1401713015" MODIFIED="1491098493701" TEXT="Sqlite-Support">
<node CREATED="1491098494497" ID="ID_1565133197" MODIFIED="1491098499291" TEXT="ist wohl noch sehr vorl&#xe4;ufig"/>
<node CREATED="1491098499712" ID="ID_1643743799" MODIFIED="1491098515234" TEXT="wenn eine Sqlite-DB gefunden wird...."/>
<node CREATED="1491098515845" ID="ID_1313255620" MODIFIED="1491098524850" TEXT="produziert er jede Menge Constraint-Verletzungen"/>
<node CREATED="1491098525396" ID="ID_1962242264" MODIFIED="1491098532791" TEXT="Tip: Sqlite-DB wegl&#xf6;schen!!!"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1437693678626" ID="ID_1536988357" MODIFIED="1488423307352" POSITION="left" TEXT="Doku">
<node CREATED="1437693687650" ID="ID_1484874437" MODIFIED="1437693692821" TEXT="Sound-Systeme">
<node CREATED="1437693693617" ID="ID_955932218" LINK="https://wiki.debian.org/Sound" MODIFIED="1437693739404" TEXT="siehe die Debian-&#xdc;bersichtsseite">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...im Besonderen die guten Diagramme f&#252;r Pulse, ALSA und Jack
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1439176872457" HGAP="15" ID="ID_355008543" MODIFIED="1488423307355" POSITION="left" TEXT="Plattform" VSHIFT="41">
<icon BUILTIN="go"/>
<node CREATED="1439176875682" HGAP="47" ID="ID_1487331591" MODIFIED="1488423307355" TEXT="Debian/Jessie" VSHIFT="60">
<icon BUILTIN="button_ok"/>
<node CREATED="1439176890840" ID="ID_170863947" MODIFIED="1488423307364" TEXT="Probleme">
<icon BUILTIN="button_ok"/>
<node CREATED="1439176900293" ID="ID_949460307" MODIFIED="1439176911529" TEXT="Linker rpath $ORIGIN">
<icon BUILTIN="button_ok"/>
<node CREATED="1439593020264" ID="ID_356536017" LINK="https://sourceware.org/bugzilla/show_bug.cgi?id=16936" MODIFIED="1439593303788">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bekannter Bug <b>binutils</b>&#160;<font color="#c72011">#16936</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439593578558" ID="ID_1155824773" LINK="http://issues.lumiera.org/ticket/965" MODIFIED="1439593639824">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Lumiera-Ticket <font color="#90011d">#965</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439593646645" ID="ID_917747701" MODIFIED="1439593709553">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gel&#246;st in&#160;<font color="#4c1383">4e8e63ebe</font>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...man &quot;hilft&quot; dem Linker mit
    </p>
    <p style="margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; text-indent: 0px">
      &quot;-Wl,-rpath-link=target/modules&quot;
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1439176912636" ID="ID_584884488" MODIFIED="1439644270018" TEXT="failed tests">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      laufen wieder alle
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1439176982698" HGAP="59" ID="ID_1330966528" MODIFIED="1439593733607" TEXT="Beobachtung: Meldungen im journal" VSHIFT="2"/>
<node CREATED="1439176948063" HGAP="54" ID="ID_1726494484" MODIFIED="1439609524714" TEXT="5 Thread/Parallel" VSHIFT="1">
<node CREATED="1439566266701" ID="ID_1280061419" MODIFIED="1439566273007" TEXT="h&#xe4;ngt mit ulimit zusammen">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1439566274091" ID="ID_1628790738" MODIFIED="1439566289223" TEXT="ohne ulimit gehts">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      test.sh Zeile 138
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439566868460" FOLDED="true" ID="ID_620984495" MODIFIED="1488423342671" TEXT="Untersuchung">
<node CREATED="1439566872203" FOLDED="true" ID="ID_1734454643" MODIFIED="1488423342553" TEXT="ulimit -T funktioniert nicht">
<node CREATED="1439566882418" ID="ID_410204401" MODIFIED="1439566885548" TEXT="bekanntes Problem"/>
<node CREATED="1439566886513" ID="ID_1412848120" LINK="https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=724461" MODIFIED="1439566919527">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Debian-Bug <font color="#9b0226">#724461</font>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1439588367804" FOLDED="true" ID="ID_1494433818" MODIFIED="1488423342553">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nebenbei <i>ohweh:</i>
    </p>
    <p>
      ulimit -t 1 ist wirkungslos
    </p>
  </body>
</html></richcontent>
<node CREATED="1439592731687" ID="ID_715556229" MODIFIED="1439592767460">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Christian:&#160;&#160;bash -c &quot;ulimit -t 1; while :; do :; done&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439592774041" ID="ID_1603492676" MODIFIED="1439592810442" TEXT="ist reine CPU-Zeit">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und wir verbringen unsere Zeit mit contention
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1439176927657" ID="ID_453561058" MODIFIED="1439609178147" TEXT="EntryID">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ist klar, hab ich gebrochen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1439608908133" ID="ID_1023076054" MODIFIED="1439608916216" TEXT="Problem mit der Hash-Funktion"/>
<node CREATED="1439609043316" ID="ID_1585741290" MODIFIED="1439609051775" TEXT="hatte ich schon mal untersucht"/>
<node CREATED="1439608917061" ID="ID_1631109794" LINK="http://issues.lumiera.org/ticket/587" MODIFIED="1439609040794">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      siehe Ticket <font color="#991130">#587</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439609055443" ID="ID_325526736" MODIFIED="1439609097840" TEXT="Problem hat sich versch&#xe4;rft">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Kollisionen jetzt bereits nach 4000 lfd. Nummern
    </p>
    <p>
      Vorher hatte ich erste Kollisionen nach 25000 Nummern
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439609098853" ID="ID_1076015737" MODIFIED="1439609125633">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erinnere mich an den
    </p>
    <p>
      guten alten &quot;Knuth-Trick&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439609127137" ID="ID_509487220" MODIFIED="1439609175040">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wow: es gen&#252;gt,
    </p>
    <p>
      die letzten beiden Zeichen mit der Knuth-Konstante zu spreizen,
    </p>
    <p>
      und ich komme locker auf 100000 Nummern ohne Kollision
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1439176963604" ID="ID_582047980" MODIFIED="1439176971256" TEXT="test-lib nicht zu debuggern">
<node CREATED="1439177141197" ID="ID_140380975" MODIFIED="1488423307368" TEXT="Segfault in GDB">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Aug 10 04:51:39 flaucher kernel: gdb[8234]: segfault at 7ffe3fa79f50 ip 0000000000718b95 sp 00007ffe3fa79f40 error 6 in gdb[400000+574000]
    </p>
    <p>
      Aug 10 04:51:39 flaucher kernel: traps: test-suite[8249] trap int3 ip:7ffff7deb241 sp:7fffffffe5c8 error:0
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1439477348298" ID="ID_1975408018" MODIFIED="1439514690123" TEXT="heruntergedampft auf einen Aufruf">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439491824537" ID="ID_1765691004" MODIFIED="1439514686397">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      function gebunden an ein lambda
    </p>
    <p>
      wobei ein Argument-Typ als vom Template-Argument
    </p>
    <p>
      der umschlie&#223;enden Funktion aufgegriffen wird
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1439492258423" FOLDED="true" ID="ID_1751368693" MODIFIED="1488423342671" TEXT="Plan">
<node CREATED="1439492266470" ID="ID_730637506" MODIFIED="1439514683843" TEXT="reproduzieren auf einer sauberen VM">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439492277444" ID="ID_840779753" LINK="http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=795445" MODIFIED="1439514718652">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Bugreport f&#252;r Debian/Jessie <font color="#e02f0a">#795445</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439492285635" FOLDED="true" ID="ID_1706807629" MODIFIED="1488423342553" TEXT="gdb-Version untersuchen">
<node CREATED="1439516472396" FOLDED="true" ID="ID_1002859575" MODIFIED="1488423308121" TEXT="backports">
<node CREATED="1439516512684" ID="ID_1031030614" MODIFIED="1439516547964">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Git: debBild/<b>Gdb_DEB.git</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439516477008" FOLDED="true" ID="ID_1182795741" MODIFIED="1488423308105" TEXT="Ver 7.8.2">
<icon BUILTIN="button_ok"/>
<node CREATED="1439516495534" ID="ID_413946117" MODIFIED="1439516594092" TEXT="Bau-Abh&#xe4;ngigkeiten">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bison dejagnu flex gobjc libncurses5-dev libreadline-dev liblzma-dev libbabeltrace-dev libbabeltrace-ctf-dev python3-dev
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1439519150614" ID="ID_644868114" MODIFIED="1439519167562">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>dutzende</i>&#160;Tests scheitern
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1439519171155" ID="ID_509605117" MODIFIED="1439519260555" TEXT="das scheint nicht ungew&#xf6;hnlich zu sein">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      verr&#228;terrischer Code im debian/rules
    </p>
    <p>
      check-stamp:
    </p>
    <p>
      ifeq ($(run_tests),yes)
    </p>
    <p>
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;$(MAKE) $(NJOBS) -C $(DEB_BUILDDIR)/gdb check \
    </p>
    <p>
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;|| echo &quot;**Tests failed, of course.**&quot;
    </p>
    <p>
      endif
    </p>
    <p>
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;touch $@
    </p>
    <p>
      
    </p>
    <p>
      <b>au weia</b>&#160;LEUTE!
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#f7f2b7" CREATED="1439521655505" ID="ID_746337758" MODIFIED="1439521712263" TEXT="funktioniert, kein Segfault mehr">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node CREATED="1439492295754" ID="ID_199817664" MODIFIED="1439492300517" TEXT="ggfs. upstream reporten"/>
<node CREATED="1439492301698" FOLDED="true" ID="ID_1431884134" MODIFIED="1488423342553" TEXT="workaround">
<node CREATED="1439492309920" ID="ID_1258344281" MODIFIED="1439492325419" TEXT="andere gdb-version">
<icon BUILTIN="help"/>
</node>
<node CREATED="1439492315080" ID="ID_45132365" MODIFIED="1439492323111" TEXT="clang verwenden">
<icon BUILTIN="help"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1439177191998" ID="ID_1188941582" MODIFIED="1439177199595" TEXT="Syslog nicht mehr STDOUT">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1439655684119" ID="ID_1194655899" MODIFIED="1439655752977" TEXT="Warnungen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      speziell: unused-function bei dem Trick mit dem std::hash macht mir Sorgen.
    </p>
    <p>
      
    </p>
    <p>
      und tats&#228;chlich: das <i>ist</i>&#160;daneben, GCC hat Recht!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439644339480" ID="ID_239239923" MODIFIED="1447996283705" TEXT="Lumiera DEB">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aktualisieren und neu bauen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439644368572" ID="ID_106785551" MODIFIED="1447996291004" TEXT="Doku: Referenz-System">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446505444775" HGAP="106" ID="ID_721807850" MODIFIED="1446505465178" TEXT="Buildsystem" VSHIFT="-5">
<node CREATED="1446505367113" ID="ID_335245102" LINK="http://issues.lumiera.org/ticket/971" MODIFIED="1446505490390">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      standard hardening-flags setzen <font color="#ba015b">#971</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1446509415481" ID="ID_180589749" MODIFIED="1488423307369" TEXT="Versionen hochsetzen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      w&#228;hle Kompatibilt&#228;t genau so, da&#223; Ubuntu-Trusty noch unterst&#252;tzt wird.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1446509422423" ID="ID_1328716100" MODIFIED="1446516746079" TEXT="gtkmm-3.0 3.10">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446509430758" ID="ID_371171235" MODIFIED="1446516743246" TEXT="glibmm-2.4 2.39">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446509601271" ID="ID_1477664506" MODIFIED="1447996256363" TEXT="libglib2.0 2.40">
<icon BUILTIN="button_ok"/>
<node CREATED="1446510347931" ID="ID_559552800" MODIFIED="1446510354349" TEXT="direkten Check hierf&#xfc;r"/>
<node CREATED="1446510354914" ID="ID_1787485177" MODIFIED="1446510366196" TEXT="check f&#xfc;r gthread fraglich"/>
</node>
<node CREATED="1446509752707" ID="ID_1392554051" MODIFIED="1446509764365" TEXT="libgdl-3-dev 3.12">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446509983556" ID="ID_366391443" MODIFIED="1446516706132" TEXT="libcairomm-1.0-dev 1.10">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446510475202" ID="ID_514495017" MODIFIED="1446516657520" TEXT="librsvg2-dev 2.30">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446511275302" ID="ID_359693763" MODIFIED="1446516636391" TEXT="libgavl-dev 1.4">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446510776857" ID="ID_674774779" MODIFIED="1446516531133" TEXT="Kommentar: librt geh&#xf6;rt zu libc 6">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446511125266" ID="ID_1198662071" MODIFIED="1446516619027" TEXT="Kommentar zu xv, x11 und xext : TODO videodisplayer obsolet?">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1447566875500" HGAP="35" ID="ID_773580451" MODIFIED="1488423307369" TEXT="w&#xfc;nschenswert..." VSHIFT="16">
<icon BUILTIN="bell"/>
<node CREATED="1447566892252" ID="ID_649638170" MODIFIED="1447566949332" TEXT="generische Compiler-Schalter zur Diagnose loggen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...damit man auch im Paketbau-Build-Output wenigstens einmal <i>alle</i>&#160; generischen Platform-Schalter sieht
    </p>
    <p>
      Ich meine also: zu Beginn vom Build sollte das Buildsystem einmal eine Infozeile ausgeben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447566892253" ID="ID_64485556" MODIFIED="1447566985705" TEXT="clean soll .sconf_temp und *.pyc mit wegr&#xe4;umen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn die st&#246;ren jeweils beim erzeugen eines Hotfix/Patch im Paketbau per dpkg --commit
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1447978374453" HGAP="140" ID="ID_156552320" MODIFIED="1488423307371" TEXT="Compiler" VSHIFT="-10">
<icon BUILTIN="prepare"/>
<node CREATED="1447978472591" ID="ID_281213456" MODIFIED="1447978479035" TEXT="GCC-4.9">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1447978381986" ID="ID_378439500" MODIFIED="1447991011184" TEXT="GCC-5">
<icon BUILTIN="flag"/>
<node CREATED="1447983098958" ID="ID_291439119" MODIFIED="1447983115489">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      deprecated: <b>auto_ptr</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447990971420" ID="ID_1499023407" MODIFIED="1447991003636">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Tests mit TypeIDs <font color="#ed0b08">scheitern</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="clanbomber"/>
</node>
<node CREATED="1447983008594" ID="ID_8361302" MODIFIED="1447990965857" TEXT="L&#xf6;sung f&#xfc;r std::hash finden">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1447978388930" ID="ID_78828927" MODIFIED="1447978395115" TEXT="Clang 3.5">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446480582168" HGAP="46" ID="ID_554745278" MODIFIED="1488423307372" TEXT="Release" VSHIFT="-5">
<icon BUILTIN="go"/>
<node CREATED="1446480601381" ID="ID_1870415321" MODIFIED="1488423307373" TEXT="release prep: clean-up obsolete information">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Doku durchk&#228;mmen nach M&#252;ll
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-1"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446482192807" ID="ID_852733328" MODIFIED="1446489451006" TEXT="Debian">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hier nach offensichtlich obsoleter Info checken
    </p>
    <p>
      <font color="#d40222">WICHTIG</font>: keine vorgreifende Infor publizieren!!!!!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446482445325" ID="ID_1134936512" MODIFIED="1446489442748" TEXT="Build-Tutorial">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die explizit angegebenen Paketnamen schon mal vorchecken
    </p>
    <p>
      die Abschnitte zu den LIbraries pr&#252;fen / umschreiben
    </p>
    <p>
      insgesamt sorgf&#228;ltig durchlesen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1446489425420" ID="ID_1155768637" MODIFIED="1446489430943" TEXT="building from source"/>
<node CREATED="1446489431443" ID="ID_1672115205" MODIFIED="1446489435759" TEXT="building the debian way"/>
<node CREATED="1446489436395" ID="ID_52990160" MODIFIED="1446489439070" TEXT="contributing"/>
</node>
<node CREATED="1446482485648" ID="ID_718218074" MODIFIED="1446483382066" TEXT="technical/build/Dependencies">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446480725508" ID="ID_789754974" MODIFIED="1488423307373" TEXT="release prep: bump version number">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      knappe Kennzeichnung des Releases in den Kommentar
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-2"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446480787391" ID="ID_1824008353" MODIFIED="1446480798862" TEXT="admin/scons/Setup.py"/>
<node CREATED="1446480787392" ID="ID_1638555946" MODIFIED="1446480803790" TEXT="data/config/setup.ini"/>
<node CREATED="1446480787392" ID="ID_619067196" MODIFIED="1446480810516" TEXT="doc/devel/Doxyfile"/>
<node CREATED="1446480787392" ID="ID_193733835" MODIFIED="1446480819004" TEXT="doc/devel/Doxyfile.browse"/>
</node>
<node CREATED="1446481181543" ID="ID_1995398229" MODIFIED="1488423307374" TEXT="R&#xfc;ck-Merge vom Release-Zweig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hier geht es darum, Konsistenz im Git herzustellen.
    </p>
    <p>
      Wenn alles korrekt gemacht wurde, d&#252;rfte es hier keinen R&#252;ckflu&#223; von &#196;nderungen geben.
    </p>
    <p>
      Bitte auch daran denken, zuerst den DEB-Zweig zu pr&#252;fen. Diesen aber nicht zur&#252;ckmergen,
    </p>
    <p>
      denn wir wollen keine DEB-Info im Master haben!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-3"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446481302159" ID="ID_1330024662" MODIFIED="1447567060657" TEXT="Release-Zweig">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481305102" ID="ID_1191774545" MODIFIED="1447567063510" TEXT="lfd Fixes von den DEBs">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481314669" ID="ID_477388235" MODIFIED="1447996222578" TEXT="Plattform-Abh&#xe4;ngigkeiten checken">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446480963028" ID="ID_83763074" MODIFIED="1488423307374" TEXT="Release-commit: Pre-release 0.pre.03">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      einzeilige Kennzeichnung wiederholen
    </p>
    <p>
      die unmittelbaren Release-Dokumente durchgehen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-4"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446481123991" ID="ID_1319204482" MODIFIED="1446481126435" TEXT="README"/>
<node CREATED="1446481126974" ID="ID_537666758" MODIFIED="1446481128522" TEXT="AUTHORS">
<node CREATED="1446481146548" ID="ID_1310945179" MODIFIED="1446481154546" TEXT="GTK-UI">
<icon BUILTIN="help"/>
</node>
<node CREATED="1446481155762" ID="ID_290411770" MODIFIED="1446481160406" TEXT="setup.ini">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1446481129206" ID="ID_799927356" MODIFIED="1446481131809" TEXT="LICENSE"/>
</node>
<node CREATED="1446481516578" ID="ID_552450474" MODIFIED="1447992311234" TEXT="Release-Zweig: upgrade current release to 0.pre.03">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Merge-commit auf den Release-Zweig.
    </p>
    <p>
      Sollte konfliktfrei sein
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-5"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481591568" ID="ID_1120064498" MODIFIED="1488423307375" TEXT="Paketieren">
<icon BUILTIN="full-6"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446481606134" ID="ID_212330450" MODIFIED="1446515946971" TEXT="Merge release -&gt; deb">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481618196" ID="ID_1882691551" MODIFIED="1446515950202" TEXT="Paket-Beschreibung">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481637738" ID="ID_1002196753" MODIFIED="1446515972779" TEXT="Manpages">
<icon BUILTIN="bell"/>
</node>
<node CREATED="1446481646057" ID="ID_262549512" MODIFIED="1446515957896" TEXT="control">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481655743" ID="ID_269337100" MODIFIED="1446515960028" TEXT="rules">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446481713984" ID="ID_1112453089" MODIFIED="1488423307377" TEXT="ausliefern">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t bauen und hochladen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-7"/>
<node CREATED="1447566826434" HGAP="0" ID="ID_808634666" MODIFIED="1447990847522" TEXT="Bugfix-Release 0.pre.03-2" VSHIFT="-1">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1447566826434" HGAP="4" ID="ID_1582615496" MODIFIED="1447990857409" TEXT="Bugfix-Release 0.pre.03-3" VSHIFT="-14">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1446482059174" ID="ID_1294670881" MODIFIED="1488423307378">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>Referenz</b>: Debian/Jessie (stable) : i386 and x86_64
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1447566711958" ID="ID_450859594" MODIFIED="1447567131949" TEXT="alte version war broken"/>
<node CREATED="1447566721616" ID="ID_520179523" MODIFIED="1447566730787" TEXT="32 / 64bit Probleme"/>
<node CREATED="1447566743461" ID="ID_1754275668" LINK="http://issues.lumiera.org/ticket/973" MODIFIED="1447566802450">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Probleme mit der Compile-Reihenfolge&#160; <font color="#d41807">#973</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447566806556" ID="ID_708047854" MODIFIED="1447566823633" TEXT="neues Bugfix-Release">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446482059174" ID="ID_1693967692" MODIFIED="1488423307379" TEXT="Ubuntu/Trusty (14.LTS) : i386 and x86_64">
<icon BUILTIN="button_ok"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1446509774664" ID="ID_458867123" MODIFIED="1446509787931" TEXT="Ausnahmen f&#xfc;r libgdl-3-dev 3.8"/>
<node CREATED="1447567139480" ID="ID_183954019" MODIFIED="1447567151655" TEXT="-&gt; diese Ausnahmen in Paketdefinition &#xfc;bernommen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1447971961407" ID="ID_1876285814" MODIFIED="1447971977858" TEXT="habe dann aber doch gdl 3.16 gebackported">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1447971982360" ID="ID_1118580069" MODIFIED="1447972076993" TEXT="Beobachtung: Paket l&#xe4;&#xdf;t sich nicht installieren">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1447971995582" ID="ID_1170070076" MODIFIED="1447972670793" TEXT="Doku-Registrierung scheitert">
<icon BUILTIN="clanbomber"/>
</node>
<node CREATED="1447972005940" ID="ID_876035298" MODIFIED="1447972073780" TEXT="doc-base Definition">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...f&#252;hrt sowohl eine README, alsauch ein Verzeichnis /usr/share/doc/lumiera/html auf, das (noch) nicht existiert
    </p>
    <p>
      unter Debian/Jessie wird das ignoriert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447972547396" ID="ID_1950291699" MODIFIED="1447972685007">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      stelle fest: Fehler auf Trusty,
    </p>
    <p>
      nur Warnung auf Mint
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, da&#223; ich versuchen kann, das Problem erst mal &quot;unter den Teppich zu kehren&quot;
    </p>
    <p>
      Die Wahrscheinlichkeit, da&#223; <i>irgend jemand</i>&#160;Lumiera unter Ubuntu/Trusty <i>installieren </i>m&#246;chte, erscheint mir akademisch
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="bell"/>
</node>
</node>
</node>
<node CREATED="1446482059174" ID="ID_1906801117" MODIFIED="1447978241196" TEXT="Ubuntu/Vivid (15.04) : i386">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446482059175" ID="ID_1107419581" MODIFIED="1488423307381" TEXT="Ubuntu/Wily (15.10) : x86_64">
<icon BUILTIN="button_cancel"/>
<node CREATED="1447978249148" ID="ID_218315533" MODIFIED="1447978290271">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bauen mit gcc-5 <b><font color="#d90d06">scheitert</font></b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447978292199" ID="ID_1160042901" MODIFIED="1447978513225" TEXT="ein Problem mit std::hash - Spezialisierung">
<icon BUILTIN="info"/>
</node>
<node CREATED="1447983052852" ID="ID_1130315053" MODIFIED="1447983076603" TEXT="liegt nur an fehlendem #include &lt;utility&gt;">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      in lib/hash-standard.hpp
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447990722939" ID="ID_1163368707" MODIFIED="1447990755430">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mit gcc-5 gebaute Tests <b><font color="#d40262">scheitern</font></b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447990760017" ID="ID_413820913" MODIFIED="1447990801156">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bauen mit gcc-4.9 <i>nicht m&#246;glich</i>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es gibt Probleme beim Linken mit den Boost-Libraries, die auf Ubuntu/wily mit gcc-5 gebaut sind.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1447990802803" ID="ID_1667893677" MODIFIED="1447990809916" TEXT="ABBRUCH"/>
</node>
<node CREATED="1446482059175" ID="ID_904094860" MODIFIED="1447978537433" TEXT="Mint/Rafaela (17.2.LTS) : x86_64">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446482249336" ID="ID_1088469455" MODIFIED="1447990825514" TEXT="Web/Doku aktualisieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Wichtig: hier nur was wirklich gebaut ist und funktioniert!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446589479832" ID="ID_1851762530" MODIFIED="1488423307382" TEXT="alte Repos wegwerfen">
<icon BUILTIN="button_ok"/>
<node CREATED="1446589487798" ID="ID_1080472422" MODIFIED="1447995228489" TEXT="Lumi-Depot: Wheezy">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446589495445" ID="ID_1083044545" MODIFIED="1447990889351" TEXT="VBox Wheezy">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446589502132" ID="ID_271504207" MODIFIED="1447990893689" TEXT="VBox Mint Quiana">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446481892624" ID="ID_1020531472" MODIFIED="1447996207720" TEXT="Ticket schlie&#xdf;en">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1439842359711" ID="ID_1982964862" MODIFIED="1446480466245" TEXT="Paket">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1439842379420" ID="ID_1336697213" MODIFIED="1439842385655" TEXT="gtk-Abh&#xe4;ngigkeiten"/>
<node CREATED="1446486970358" ID="ID_1758732894" MODIFIED="1446487031892" TEXT="was ist mit libXV">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eigentlich war die nur notwendig f&#252;r das Video-Viewer Widget,
    </p>
    <p>
      was nun leider tot ist. Wir haben noch keinen Ersatz. Deshalb lasse ich die Abh&#228;ngigkeit
    </p>
    <p>
      bestehen, aber irgendwann m&#252;ssen wir das schon glattziehen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1446505367113" ID="ID_1119934206" LINK="http://issues.lumiera.org/ticket/971" MODIFIED="1446505429946">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hardening-flags! <font color="#ba015b">#971</font>
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#eee5c3" COLOR="#990000" CREATED="1478387336953" ID="ID_1320541285" MODIFIED="1488423307382" TEXT="debian/copyright">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1478387345624" ID="ID_666713431" MODIFIED="1478387376477" TEXT="sollte alle Copyright-statements korrekt zusammenfassen"/>
<node CREATED="1478387362709" ID="ID_1001009846" MODIFIED="1478387376477" TEXT="siehe Debian-Policy"/>
</node>
</node>
</node>
<node CREATED="1446356359992" HGAP="9" ID="ID_1850896628" MODIFIED="1446480440058" POSITION="left" TEXT="QA" VSHIFT="69">
<icon BUILTIN="go"/>
<node CREATED="1447985496639" ID="ID_1191711297" MODIFIED="1447985500927" TEXT="untersuchen">
<node CREATED="1447985501798" ID="ID_732609615" MODIFIED="1488423307383" TEXT="std::hash-Spezialisierung">
<node CREATED="1447985531602" ID="ID_515281257" LINK="http://issues.lumiera.org/ticket/722" MODIFIED="1447985548240">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ticket <font color="#d40230">#722</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447985552663" ID="ID_1932953869" MODIFIED="1447985590246" TEXT="Hack ist inzwischen obsolet">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      seit gcc-4.8 ist kein static_assert mehr in der STDlib
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447985591922" ID="ID_800130211" MODIFIED="1447985636328" TEXT="Frage: wie kann ich ein SFINAE-Br&#xfc;cke bauen"/>
<node CREATED="1447985637132" ID="ID_1680970111" MODIFIED="1447985646206" TEXT="Problem: wir haben nur einen Typparameter"/>
</node>
</node>
<node CREATED="1447567187010" ID="ID_1008538054" MODIFIED="1447567220394" TEXT="sp&#xe4;ter...">
<icon BUILTIN="bell"/>
<node CREATED="1447567193361" ID="ID_661445245" MODIFIED="1488423307384" TEXT="preliminary optimization">
<icon BUILTIN="ksmiletris"/>
<node CREATED="1447566743461" ID="ID_1971297499" LINK="http://issues.lumiera.org/ticket/973" MODIFIED="1447566802450">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Probleme mit der Compile-Reihenfolge&#160; <font color="#d41807">#973</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
<node CREATED="1477665256799" ID="ID_685276209" MODIFIED="1488423307384" TEXT="&#xc4;rger">
<icon BUILTIN="smily_bad"/>
<node CREATED="1477665276421" ID="ID_714740974" MODIFIED="1477665294382" TEXT="dlclose #1034">
<node CREATED="1477665304137" ID="ID_65862146" MODIFIED="1477665319167" TEXT="Assertion, da Library nicht entladen werden kann">
<icon BUILTIN="info"/>
</node>
<node CREATED="1477665320934" ID="ID_1841226938" MODIFIED="1477665322506" TEXT="LD_DEBUG=all NOBUG_LOG=pluginloader_dbg:TRACE  2&gt;&amp;1 target/lumiera | c++filt &gt;weia"/>
<node CREATED="1477665330317" ID="ID_1760958407" MODIFIED="1477665341818" TEXT="weitere Untersuchung notwendig....">
<icon BUILTIN="bell"/>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1446356368070" ID="ID_768449868" MODIFIED="1447996318860" TEXT="Au Au"/>
</node>
</node>
</map>
