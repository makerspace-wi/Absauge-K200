# Absauge-RL 200
![3013-reinluftabsauggeraet-rl200-felder-feldergroup_1_-_media--3875e560--query](https://user-images.githubusercontent.com/42463588/149331293-7ce102f2-bbdc-48d4-a309-3f767487afd5.png)
<ul>
<li>Volumenstrom max.: 4000 m³/h</li>
<li>Nennvolumenstrom: 20 m/sek, 2300 m³/h</li>
<li>Unterdruck: 2380 Pa</li>
<li>Filterfläche: 20 m²</li>
<li>Absauganschluss-Durchmesser: 200 mm</li>
<li>Reststaubgehalt: < 0,1 mg/m3</li>
</ul>

[Link zu FELDER](https://www.felder-group.com/de-at/produkte/absauggeraete-entstauber-c1963/reinluftabsauggeraet-entstaubungsanlage-performance-line-p144306)
<br><br>
Seit Ende 2021 is der Makerspace Wiesbaden e.V. stolzer Besitzer einer professionellen Entstaubungsanlage RL 200 von der Firma Felder.
<br> 
Die Maschine wurde vom hessischen Wirtschaftsministerium aus Lotteriegeldern finanziert und die notwendigen Anschlussrohre zu den 5 Maschinen von ['Anstiftung'](https://anstiftung.de/).
<br><br>
Damit wurde das Ende unserer alten, kleinen Absauge mit BlastGate Steuerung eingeläutet.
<br>
Die fast 400kg schwere Absauge wurde im kleinen Abstellraum unter der Treppe platziert, dazu musste eine Ständerwand aufgebrochen werden.

<img width="400" src="https://user-images.githubusercontent.com/42463588/149338032-6201faf3-45aa-4256-8af1-6e30a7d6d1b6.jpg">

Von der Absauge gehen Rohre - abgehängt von der Decke - an die Formatkreissäge, an den Hobel, an die Bandsäge, an den Schleifer und an die Kappsäge.

<img width="400" src="https://user-images.githubusercontent.com/42463588/149339016-24f99a33-7140-4084-9818-c4530916bdf3.jpg">
<img width="400" src="https://user-images.githubusercontent.com/42463588/149620149-f1ec225c-32f6-4462-bbde-fdf8c8975d15.jpg">
<br>
Jeder Maschinenanschluss hat einen eignen elektropneumatischen Schieber, der von der jeweiligen Maschinensteuerung geöffnet wird, sobald die Maschine anläuft und um ca. 10 Sekunden verzögert wieder schließt, wenn die Maschine ausgeschaltet wird.

Die benötigte Pressluft, die auch für die automatisierte Reinigung der Filterfläche benötigt wird, kommt von einem kleinen Kompressor, der neben der Absauge steht.


![Absauge Schema](https://user-images.githubusercontent.com/42463588/149550942-25aaf54e-17d3-4d47-8f5c-0e50b0c29078.png)

Im folgenden die grobe Funktionsbeschreibung - dazu bitte das 
[Funktionsschema](doc/Absauge%20Schema.pdf) öffnen
# Ablauf Einschaltvorgang
- RFID-Chip an das entsprechende Lesegerät gehalten.
- wenn die Chip-Nummer gelesen wurde, wird diese an den Systemrechner gesendet und geprüft, ob freigeschaltet werden kann.
- ist die Prüfung positiv sendet der Systemrechner das Freischaltsignal und der Strom für die entsprechende Maschine wird für X Minuten eingeschaltet.
- zu diesem Zeitpunkt wird auch der erste Eintrag in das 'Machine Log' geschrieben.
- alle absaugberechtigten Maschinen haben einen Stromsensor, der dem Systemrechner meldet, sobald die Maschine angelaufen ist.
- dies löst einen Eintrag in der Betriebsstunden Datenbank aus und die Maschinenvariable 'operating' auf dem Systemrechner wird auf '1' gesetzt - desweiteren wird das elektropneumatische Ventil für die Maschine bestromt und gibt das Rohr zur Absauge frei.
- auf dem Systemrechner sorgt ein 'event' ausgelöstes Script dafür, dass bei jeder Änderung der 5 'operating' Variablen (MA05 - MA09) eine weitere Variable 'any_machine_on' auf 'true' gesetzt wird, wenn eine oder mehrere Maschinen laufen. 
- ändert sich 'any_machine_on' von 'false' auf 'true' sendet der Systemrechner das Einschaltsignal für unsere Absauge, an deren Controller.
- sind alle Maschinen ausgeschaltet, sorgt das voher erwähnte Script dafür, dass die Varialble 'any_machine_on' auf 'false' geht und die Nachlaufzeit von momentan 15 Sekunden gestartet wird, die - falls in der Zeit nicht wieder eine Maschine eingeschaltet wurde - dann den Befehl zum Ausschalten an den Controller der Absauge sendet.
- die Maschinencontroller schließen die pneumatische Klappe ca. 10 Sekunden nach dem Abschalten der Maschine.
