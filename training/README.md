# Ocropus Trainingsdaten Fraktur

https://github.com/tmbdev/ocropy/wiki

https://github.com/jze/ocropus-model_fraktur

http://www.deutschestextarchiv.de/

Trainingstexte mit korrekten Umbrüchen

## Installation für Training mit Calamari

* Installation Anaconda mit `tensorflow_gpu` 2.0.0
* Zusätzlich war Installation von `libcuda1` erforderlich
* Anaconda-Umgebung aktivieren:
  <pre>
  source <condaDir>/bin/activate
  </pre>
* Installation von virtueller Umgebung für Calamari (falls gewünscht) und
  aktivieren:
  <pre>
  conda env create -f environment_master_gpu.yml
  conda activate ...
  </pre>
  Alternativ Calamari direkt in Anaconda-Umgebung installieren.

## Training mit Ocropus-Daten

<pre>
calamari-train --files ocropus-model_fraktur-master/training/*.bin.png \
               --output_dir=myModel/ \
               --validation=ocropus-model_fraktur-master/testing/*.bin.png
</pre>
Achtung: Testdaten als Validation-Daten missbraucht!

ca. 30'000 Schritte

Persönlicher Eindruck: Dieses Modell ist genauer als das mitgelieferte
Camari-Modell (zumindest bei Verwendung ohne Voting)
TODO: Training Voting-Modell

## OCR

Derzeitiger Workflow:

* Vorbehandlung mit Scantailor (Ausgabe 600 dpi, 1 bit Farbtiefe)

* Skalieren auf 50%
  <pre>
  convert -scale 50% -depth 1 srcImage tgtImage
  </pre>

* Erzeugen der Zeilenboxen
  * kraken:
  <pre>
  kraken -i imageFile segmentFile segment
  </pre>
  
  * Tesseract (Installation mit Trainingssupport erforderlich)
  <pre>
  tesseract imageFile baseFilename lstmbox
  </pre>
  Umwandlung der Boxfiles in JSON-File wie kraken

* Vorhersage mit Calamari
  <pre>
  calamari-predict --checkpoint myModel/best.ckpt \
    --files lineImageFiles
  </pre>

* Zusammensetzen der Zeilen mit Heuristik

* Vergleich mit Tesseract-Vorhersage und evt. Modell anpassen


* Zeilen zusammensetzen (eigenes Tool)

<pre>
kraken-join-lines.py segmentFile.json textFile.txt -d imgLinesDir
</pre>

## Modell anpassen

* Voraussetzung: Bilder von Einzelzeilen (1 Bit Farbtiefe) +
Ground-Truth-Textdatei mit gleichem Basisnamen
<pre>
calamari-train --weights myModel/best.ckpt \
               --output_dir=improved/ \
               --files myTraining/*.png
               --validation=myValidation/*.png
</pre>


