# bike-kirmes
Beleuchtung fürs Fahrrad mit verschiedenen bunten modi

# Modi
### Schalter 1 
- Wechselt zwischen den von dir festgelegten Modi durch
### Schalter 2 (Überschreibt Schalter 1)
- Blinker eine Seite
### Schalter 3 (Überschreibt Schalter 1)
- Blinker andere Seite
### Schalter 4 (Überschreibt Schalter 1-3)
- Ein/Aus

(Der Code für ein animierten Blinker ist vorhanden, aber nicht für 2 aneinanderhängende Segmente optimiert)

# Partslist
|Bauteil|z.B. von |~Preis|
|--|--|-|
|1m WS2812b 144led/m | AliExpress | 10€ |
|Alu U-Profil 2x 50cm x 17,5mm | AliExpress | 5€ |
|Arduino Pro Micro | Ebay | 5€ |
|Motorad Lenker Blinker Schalter | Ebay | 5€ |
|Platine, JST-Stecker und Kabel|AliExpress|5€|
|Halterung für Alu-Profile|3D-Drucker (stl & 3mf hängt an)|30ct|
|Lenkererweiterung als befestigung für den Schalter|AliExpress|4€|

# Was kommt wo dran?
|Bauteil|Pin|
|-|-|
|LEDs|5|
|Schalter 1 (Modus Auswählen)|7|
|Schalter 2 (Blinker 1)|0|
|Schalter 3 (Blinker 2)|3|
|Schalter 4 (An/Aus)|1|

![hookup](https://user-images.githubusercontent.com/24511715/146252441-660fef14-19ac-4e3d-aa8d-00fcd79b262b.png)

# LEDs anschließen
Der Code sieht es vor, dass beide LED-Streifen hintereinander hängen.
Bedeutet: Der DatenAusgang der letzten LED des ersten Streifens muss an den Dateneingang der ersten LED des zweiten Streifens angeschlossen werden.

# Code
## Was muss im Code angepasst werden?
- Gesamtanzahl der LEDs (#define NUM xxx)
- Aufteilung der LEDs auf die beiden Segmente (int segment_[1/2] = xx)
- Welche Modi es mit welchen Einstellungen in welcher Reihenfolge gibt
  - Geschwindigkeit der Effekte (int x_delay ist die Pause zwischen Aktualisierungen der LEDs im Effekt x. Je kürzer, desto schneller.)
  - Farben (int x_color[3] {r,g,b,} ist die Farbe des Effekts x mit den Werten r,g,b von je 0-255)
  - Helligkeit (0-255)

# Sonstiges
## Montierung
### LEDs
Ich habe eine einfache 3D-Druckbare Halterung entworfen, in die die U-Profile mit einer Breite von 17,5mm einfach eingeklebt werden können. Diese können dann am Fahrrad an den Montagepunkten für Gepäckträger festgeschraubt werden. Ggf muss die Halterung mit einem Feuerzeug o.ä. leicht angewärmt werden, damit sich die Halterung an krumme Winkel anpassen kann.

### Controller & Powerbank
Ich nutze eine Powerbank mit ~20.000mAh. Diese habe ich in einem 3D-Gedruckten Gehäuse zusammen mit dem Arduino und Fahrradflicken (Man weiß ja nie :) untergebracht. Dieses Gehäuse lebt jetzt in einem meiner Getränkehalter.

## Stromverbrauch
Auf größter Helligkeit löst meine Powerbank teilweise OverCurrent aus. Passe am besten deine Helligkeit an deine Powerbank an. Mit den 20.000mAh komme ich bei einer Helligkeit von ca. 150 bei ±7h raus.
