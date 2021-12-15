# bike-kirmes
Beleuchtung fürs Fahrrad mit verschiedenen bunten modi

# Modi
### Schalter 1 (Wechselt durch)
- Einhornkotze
- Color Fade
- Einfarbig (Weiß)
- Einfarbig (Rot)
- Blaulicht
- Stroboskop (Weiß)
- Warnblinker
### Schalter 2 (Überschreibt Schalter 1)
- Blinker eine Seite
### Schalter 3 (Überschreibt Schalter 1)
- Blinker andere Seite
### Schalter 4 (Überschreibt Schalter 1-3)
- aus

(Der Code für ein animierten Blinker ist vorhanden, aber nicht für 2 aneinanderhängende Segmente optimiert)

# Partslist
|Bauteil|z.B. von |~Preis|
|--|--|-|
|1m WS2812b 144led/m | AliExpress | 10€ |
|Alu U-Profil 2x 50cm 17,5mm breit |AliExpress | 5€ |
|Arduino Pro Micro | Ebay | 5€|
|Motorad Lenker Blinker Schalter | Ebay | 5€ |
|Platine, Stecker und Kabel|AliExpress|5€|
|Halterung für Alu-Profile|3D-Drucker (stl hängt an)|30ct|
|Lenkererweiterung als Halter für den Schalter|AliExpress|4€|

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
Der Code sieht es vor, dass beide LED-Streifen in Reihe geschaltet werden.
Bedeutet: Der Datenpin (Ausgang) der letzten LED des ersten Streifens muss an den Datenpin (Eingang) der ersten LED des zweiten Streifens angeschlossen werden.
# Code
## Was muss im Code angepasst werden?
- Gesamtanzahl der LEDs (#define NUM xxx)
- Aufteilung der LEDs auf die beiden Segmente (int segment_[1/2] = xx)

## Was kann im Code angepasst werden?
- Geschwindigkeit der Effekte (int x_delay ist die Pause zwischen Aktualisierungen der LEDs im Effekt x. Je kürzer, desto schneller.)
- Farben (int x_color[3] {r,g,b,} ist die Farbe des Effekts x mit den Werten r,g,b von je 0-255)

# Sonstiges
## Montierung
### LEDs
Ich habe eine einfache 3D-Druckbare Halterung entworfen, in die die U-Profile einfach eingeklebt werden kann. Diese können dann am Fahrrad an den Montagepunkten für Gepäckträger festgeschraubt werden. 
### Controller & Powerbank
Ich kann meine Powerbank und den Arduino ganz gut in meiner Satteltasche unterbringen. Alternativ könnte man sich auch ein extra Gehäuse Drucken oder das ganze z.B. in einem Flaschenhalter unterbringen.

## Stromverbrauch
Betrieben wird das ganze über USB. Bei allen LEDs auf weiß und höchster Helligkeit schaltet sich meine Powerbank wegen overcurrent von alleine ab. Bei der aktuell eingestellten Helligkeit zieht das ganze bei Einhornkotze 1-1,5A@5V. Bei Einzelfarbe (Weiß) um die 2A.
