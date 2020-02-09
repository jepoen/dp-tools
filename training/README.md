# Ocropus Trainingsdaten Fraktur


## Trainingsdaten und Modelle

* data/ocropus-train-data.zip: Original-Trainingsdaten Ocropus
  https://github.com/jze/ocropus-model_fraktur
* data/my-train-data.zip: Zusätzliche eigen Trainingsdaten
* model/ocropus_base.ckpt: Modell mit Ocropus-Daten
* model/mymodel-best.ckpt: Modell mit Ocropus- und eigenen Daten

Eigene Daten:

* Wustmann: Allerhand Sprachdummheiten.
* Rieß: Aus grauen Mauern und grünen Weiten.

Weitere mögliche Datenquellen:

* https://github.com/tmbdev/ocropy/wiki
* http://www.deutschestextarchiv.de/

## Installation für Training mit Calamari

* Installation Anaconda mit `tensorflow_gpu` 2.0.0
* Zusätzlich war Installation von `libcuda1` erforderlich
* Installation von virtueller Umgebung für Calamari:
  <pre>
  conda env create -f envirnonment_master_gpu.yml
  </pre>

Trainingsdaten von `https://github.com/jze/ocropus-model_fraktur`

<pre>
calamari-train --files ocropus-model_fraktur-master/training/*.bin.png --checkpoint_frequency=1000 --output_dir=calamari-models/ --validation=ocropus-model_fraktur-master/testing/*.bin.png --early_stopping_frequency=1000
</pre>

ca. 30'000 Schritte

TODO: Training Voting-Modell

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

