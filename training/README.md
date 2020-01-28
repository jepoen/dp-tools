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

## Modell anpassen

* Voraussetzung: Bilder von Einzelzeilen (1 Bit Farbtiefe) +
Ground-Truth-Textdatei mit gleichem Basisnamen
<pre>
calamari-train --weights myModel/best.ckpt \
               --output_dir=improved/ \
               --files myTraining/*.png
               --validation=myValidation/*.png
</pre>

## OCR

* Installation ohne Anaconda mit Python3-venv

* Installation `tensorflow=2.0.0` (ohne GPU-Support)

* Installation von `calamari_ocr`, `kraken` in dieser Umgebung

* Scantailor-Ausgabe auf 300 dpi, Tiefe 1 runterskalieren

<pre>
convert -scale 50% -depth1 srcImage tgtImage
</pre>

* Seite segmentieren mit kraken

<pre>
kraken -i imageFile segmentFile segment
</pre>

* Seite zerlegen in Zeilen (eigenes Tool)

<pre>
kraken-img-split.py segmentFile.json imageFile -d imgLinesDir
</pre>

* Vorhersage

<pre>
calamari-predict --checkpoint model.ckpt --files imgLinesDir/line-\*.png
</pre>

* Zeilen zusammensetzen (eigenes Tool)

<pre>
kraken-join-lines.py segmentFile.json textFile.txt -d imgLinesDir
</pre>

