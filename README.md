# Absauge-RL 200
Seit Ende 2021 is der Makerspace Wiesbaden e.V. stolzer Besitzer einer professionellen Entstaubungsanlage RL 200 von der Firma Felder.
<br> 
Die Maschine wurde vom hessischen Wirtschaftsministerium aus Lotteriegeldern finanziert und die notwendigen Anschlussrohre zu den 5 Maschinen von der ['Anstiftung'](https://anstiftung.de/).
<br><br>
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

Damit wurde das Ende unserer alten, kleinen Absauge mit BlastGate Steuerung eingeläutet.
<br><br>
Die fast 400kg schwere Absauge wurde im kleinen Abstellraum unter der Treppe platziert, dazu musste eine Ständerwand aufgebrochen werden.

<img width="400" src="https://user-images.githubusercontent.com/42463588/149338032-6201faf3-45aa-4256-8af1-6e30a7d6d1b6.jpg">

Von der Absauge gehen Rohre (200mm, 160mm & 100mm) - abgehängt von der Decke - an die Formatkreissäge, den Hobel, die Bandsäge, der Schleifmaschine und die Kappsäge.

<img width="300" src="https://user-images.githubusercontent.com/42463588/149339016-24f99a33-7140-4084-9818-c4530916bdf3.jpg"><img width="300" src="https://user-images.githubusercontent.com/42463588/150532144-74affb5c-32f1-4be5-aa9a-c06e933944e8.JPG">
<br>
Jeder Maschinenanschluss hat einen eignen elektropneumatischen Schieber, der von der jeweiligen Maschinensteuerung geöffnet wird, sobald die Maschine anläuft und um ca. 10 Sekunden verzögert wieder schließt, wenn die Maschine angehalten wird.

Die benötigte Pressluft, die auch für die automatisierte Reinigung der Filterfläche benötigt wird, kommt von einem kleinen, schallgedämpften Kompressor, der neben der Absauge steht.


![Absauge Schema](https://user-images.githubusercontent.com/42463588/149550942-25aaf54e-17d3-4d47-8f5c-0e50b0c29078.png)

Im folgenden die grobe Funktionsbeschreibung - dazu bitte das 
[Funktionsschema](doc/Absauge%20Schema.pdf) öffnen
# Ablauf Einschaltvorgang
- RFID-Chip an das entsprechende Lesegerät gehalten.
- wenn die Chip-Nummer gelesen wurde, wird diese an den Systemrechner gesendet und dort eingehend geprüft, ob freigeschaltet werden kann.
- ist die Prüfung positiv sendet der Systemrechner das Freischaltsignal und der Strom für die entsprechende Maschine wird für z.B. 60 Minuten eingeschaltet.
- zu diesem Zeitpunkt wird auch der erste Eintrag in das 'Machine Log' geschrieben.
- alle absaugberechtigten Maschinen haben einen Stromsensor, der dem Systemrechner meldet, sobald die Maschine angelaufen ist.
- dies löst einen Eintrag in der Betriebsstunden Datenbank aus und die Maschinenvariable 'operating' auf dem Systemrechner wird auf '1' gesetzt - desweiteren wird das elektropneumatische Ventil für die Maschine bestromt und gibt das Rohr zur Absauge frei.
- auf dem Systemrechner sorgt ein 'event' ausgelöstes Script dafür, dass bei jeder Änderung der 5 'operating' Variablen (MA05 - MA09) eine weitere Variable 'any_machine_on' auf 'true' gesetzt wird, wenn eine oder mehrere Maschinen laufen. 
- ändert sich 'any_machine_on' von 'false' auf 'true' sendet der Systemrechner das Einschaltsignal für unsere Absauge, an deren Controller.
- sind alle Maschinen ausgeschaltet, sorgt das voher erwähnte Script dafür, dass die Varialble 'any_machine_on' auf 'false' geht und eine Nachlaufzeit von momentan 15 Sekunden gestartet wird, die - falls in der Zeit nicht wieder eine Maschine eingeschaltet wurde - dann den Befehl zum Ausschalten an den Controller der Absauge sendet.
- die Maschinencontroller schließen die pneumatische Klappe ca. 10 Sekunden nach dem Abschalten der Maschine.

Das Blockschaltbild des Controllers findest du [hier](doc/Controller_Absaugung.pdf)

Der 2. Schaltkanal ist für den Luftkompressor. Dieser wird bestromt, sobald das erste Mitglied den Makerspace betritt. Dann läuft der Kompressor an und schaltet selbstständig ab, wenn 8 bar Druck erreicht sind.
<br><br>Der Kompressor wird wieder vom Stromnetz getrennt, sobald das letzte Mitglied den Makerspace verlassen hat - deshalb hier noch einmal der Aufruf an ALLE: <br><br><b>Bitte das Ausloggen nicht vergessen!</b>
<br><br>
### Controller Box
![IMG_9646](https://user-images.githubusercontent.com/42463588/150523746-c0f4072c-94b8-4402-bbb1-70139d016218.JPG)

Im folgenden eine kurze Erklärung der verschiedenen Diagnose LEDs auf dem Prozessor Board.

![IMG_9647 2](https://user-images.githubusercontent.com/42463588/150528933-095ea0e8-ef60-49d0-86c3-3c20022cdb7d.JPG)
* Power LED - leuchtet dauerhaft wenn die Controller Box bestromt ist
* System Connect LED (D1 - rot) leuchtet beim Versuch  eine Verbindung mit dem Symcon System aufzubauen und erlischt, wenn erfolgreich verbunden wurde.
Die LED blinkt, wenn nach 2 Minuten keine Verbindung aufgenommen werden konnte.
* RSSI LED (D2 - rot) die Helligkeit dieser LED signalisiert die Signalstärke des zuletzt übertragenen Paketes.
* Associate LED (D3 - grün) siehe folgende Liste
  
![Bildschirmfoto 2022-01-21 um 11 54 37](https://user-images.githubusercontent.com/42463588/150526423-c17f7973-14b3-4b70-ba1b-0bdc9e80765a.png)
Normales Blinken signalisiert die korrekte Einbindung in das Zigbee Netzwerk.