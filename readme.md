# Ecocathlon

L'Ecocathlon est un jeu concu par la [DREAL](http://www.grand-est.developpement-durable.gouv.fr/). Vous pouvez trouver toute les information le concernant sur le [site](http://www.ecocathlon.fr)

Ce programme a été écrit pour être utilisé avec le circuit conçu pour l'ecocathlon par le [Fablab MDesign](https://mdesign57.wordpress.com/).

## **Créer un module**

### **Le circuit**

Le circuit est basé sur le microcontrolleur esp8266 implementé grâce au module [ESP-WROOM-02](https://www.espressif.com/sites/default/files/documentation/0c-esp-wroom-02_datasheet_en.pdf). Le gerber file du circuit principal se trouve dans "./circuit". A se circuit principal se rajoute une batterie LI-PO de 2000mAh et un module RFID basé sur le [MFRC522](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf).

Voici le schéma de la manière de souder ces différents éléments ensemble.
![Schéma soudure](./circuit/schema.png)

### **Flasher le programme sur le circuit**

Une fois le circuit souder il faut uploader le programme sur l'esp8266. Pour cela on doit utiliser un [usb to tll converter](https://www.amazon.fr/AZDelivery-Adaptateur-FT232RL-Arduino-Adapter/dp/B01N9RZK6I/ref=rtpb_2/261-7673969-5887468?_encoding=UTF8).

L'usb to ttl converter ce connecte de cette facon.
![schéma connexion ftdi](./circuit/ftdi.png)

Lorsque le converter est connecté a l'ordinateur et aux circuit une led verte devrait etre allumée.

Pour uploader le programme on va utilisé l'outil fournis par Espressif : [esptool](https://github.com/espressif/esptool)

Attention : **esptool requiert python pour fonctionner**

On commence par télécharger esptool :  
```$ pip install esptool```

Attention : **Avant de flasher le programme sur l'esp8266 il faut le passer en mode download pour cela appuyez sur le bouton marqué 1 sur le schéma et tout en maintenant le bouton 1 appuyez sur le bouton 2 puis tout relaché**

Ensuite on upload le code sur le circuit avec :  
```$ esptool -b921600 write_flash 0x00000 "chemin du dossier"\.pio\build\esp01_1m\firmware.bin```

On pourra aussi verifier si le micro-controlleur fonctionne correctement avant de flasher le programme:  
```$ esptool chip_id```
