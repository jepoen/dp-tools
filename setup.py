from setuptools import setup

setup(
  name='dp-tools',
  packages=[],
  scripts=['bin/dp-begin-split', 'bin/ocr-convert', 'bin/ocr-collectgt',
    'bin/ocr-tessdiff', 'bin/ocr-joinpara',
  ],
  include_package_data=True,
  zip_safe=False,
)

