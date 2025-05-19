#!/bin/bash -x
zip backup`date +"%d_%m_%y"`.zip *.cpp *.shader *.sh ../*.py ../plan ../provenance ./res/*
