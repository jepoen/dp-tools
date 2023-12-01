# Ocropus Trainingsdaten Fraktur


## Trainingsdaten und Modelle

* data/ocropus-train-data.zip: Original-Trainingsdaten Ocropus
  https://github.com/jze/ocropus-model_fraktur
* data/my-train-data.zip: Zusätzliche eigen Trainingsdaten
* model/ocropus_base.ckpt: Modell mit Ocropus-Daten
* model/mymodel-best.ckpt: Modell mit Ocropus- und eigenen Daten

Eigene Daten:

* Wustmann: Allerhand Sprachdummheiten.
* Rieß: Aus grauen Mauern und grünen Weiten. Sächsischer Heimatschutz 1924.
* Wiedemann: Sprachbilder nach bestimmten Sprachregeln.
  A. Oehmigke's Verlag 1874.
* Nansen: Auf Schneeschuhen durch Grönland. Band 1. Verlagsanstalt Hamburg 1898.
* Thomas: Die letzten zwanzig Jahre deutscher Litteraturgeschichte.
  Walther Fieder 1900.
* Sächsischer Heimatschutz: Mitteilungen Band X. 1921.
* Oldenbourg: Buch und Bildung. C. H. Beck. 1925
* Clemen: Die nichtchristlichen Kulturreligionen, Teil 1 und 2. Teubner 1921.
* Hedin: Von Pol zu Pol. Teil 2. Brockhaus 1914.
* Rein: Auf der Heidecksburg. Greifenverlag 1926.
* Wunder: Chemische Unterhaltungen. Verlag Peter Oestergaard 1915.

Weitere mögliche Datenquellen:

* https://github.com/tmbdev/ocropy/wiki
* http://www.deutschestextarchiv.de/ (teilweise korrekte Zeilenumbrüche)

## Eigene Trainingsdaten Antiqua

* Volz: Der Graf von Saint-Germain. 1923.
* Pflanzen-Atlas zu Seb. Kneipp's »Waser-Kur«. 1893.
* Kästner: Vom Reisen und Reisen lassen. 1910.
* Lindworsky: Experimentelle Psychologie. 1923.
* Führer durch das Österreichische Museum für Kunst und Industrie. 1914.
* Säuberlich: Buchgewerbliches Hilfsbuch. 1921.
* Trinkler: Quer durch Afghanistan nach Indien. 1927.
* Hirschfeld: Frau Rietschel das Kind. 1925. (Enthält ſ)

## Installation für Training mit Calamari2

* Installation Cuda 11.7
* Installation Miniconda
* Aktivierung und Downgrade auf Python 3.9: `conda install python=3.9`
* Installation Calamari: `pip install calamari-ocr`

  * Falls Pakete trotz Installation nicht gefunden werden,
    mit pip deinstallieren und neu installieren
  * installierte Tensorflow-Version: 2.6
* Anaconda-Umgebung aktivieren:

  ~~~~
  source <condaDir>/bin/activate
  ~~~~

## Training mit Ocropus- und eigenen Daten

Version 2:

~~~~
calamari-train \
  --train.images mydata/fraktur/train/*.png ocropus-data/training/*.bin.png \
  --val.images mydata/fraktur/valid/*.png ocropus-data/testing/*.bin.png \
  --trainer.output_dir /data/ocr-train/modelDir \
  --early_stopping.n_to_go 5 \
  --device.gpus 0 \
  --n_augmentations=5
~~~~

Nachtrainieren:

* Voraussetzung: Bilder von Einzelzeilen (1 Bit Farbtiefe) +
  Ground-Truth-Textdatei mit gleichem Basisnamen

Nachtrainieren:

~~~~
calamari-train \
  --warmstart.model /data/ocr-train/modelDir/best.ckpt \
  --train.images gt/train/*.png \
  --val.images gt/valid/*.png \
  --trainer.output_dir modelDir \
  --early_stopping.n_to_go=5 \
  --device.gpus 0 \
  --codec.keep_loaded True \
  --n_augmentations=5
~~~~

## OCR

Derzeitiger Workflow:

* Vorbehandlung mit Scantailor (Ausgabe 600 dpi, 1 bit Farbtiefe)

* Erzeugen der Zeilenboxen mit Script `ocr-convert`
  * Herunterskalieren auf 50%
  * Erzeugen der Zeichenboxen mit Tesseract
  * Zusammensetzen zu Zeilenboxen
  * Ausschneiden der Zeilen
  * kraken:

* Vorhersage mit Calamari

  ~~~~
  calamari-predict --checkpoint /data/ocr-train/modelDir/best.ckpt \
    --data.images tmp/*/p*png --verbose False
  ~~~~

* Vergleich mit Tesseract-Vorhersage und evt. Modell anpassen

* Zeilen zusammensetzen (eigenes Tool)

  ~~~~
  ocr-joinpara segmentFile.json textFile.txt -d imgLinesDir
  ~~~~

