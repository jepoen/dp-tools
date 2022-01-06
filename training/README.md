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

Weitere mögliche Datenquellen:

* https://github.com/tmbdev/ocropy/wiki
* http://www.deutschestextarchiv.de/ (teilweise korrekte Zeilenumbrüche)

## Eigene Trainingsdaten Antiqua

* Volz: Der Graf von Saint-Germain. 1923.
* Pflanzen-Atlas zu Seb. Kneipp's »Waser-Kur«. 1893.
* Kästner: Vom Reisen und Reisen lassen. 1910.
* Lindworsky: Experimentelle Psychologie. 1923.
* Führer durch das Österreichische Museum für Kunst und Industrie. 1914.

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
                       mydata/training/*.png \
               --output_dir=myModel/ \
               --validation=ocropus-model_fraktur-master/testing/*.bin.png \
                            mydata/valid/*.png
</pre>
Achtung: Ocropus-Testdaten als Validation-Daten missbraucht!

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


