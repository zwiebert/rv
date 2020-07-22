
import { getLocaleFromNavigator, init, addMessages, _ } from 'svelte-i18n';

const en = {
    "app": {
        "zone":"Zone",
        "duration":"Dur",
        "run":"Run",
        "remaining":"Rem",
        "pump":"Pump",
        "pressControl":"PC",
        "rainSensor":"Rain-Sensor",
        "reload":"Reload",
        "restartMcu":"Restart MCU",
        "save":"Save",
        "hide":"Hide",
        "zoneName":"Name",
        "nav_main_rv":"RV",
        "nav_main_config":"Config",
        "nav_main_firmware":"Firmware",
        "nav_fw_rv":"RV-MCU",
        "nav_fw_netMcu":"Net-MCU",
        "nav_cfg_rv":"RV-MCU",
        "nav_cfg_netMcu":"Net-MCU",
        "msg_waitForMcuRestart":"Wait for MCU to restart...",
        "msg_enterNewName":"Enter new name:",
        "msg_firmwareIsUpdating":"Firmware is updating...",
    }
} ;


const de = {
    "app": {
        "zone":"Bereich",
        "duration":"Dauer",
        "run":"Ein",
        "remaining":"Rest",
        "pump":"Pumpe",
        "pressControl":"Druckschalter",
        "rainSensor":"Regen-Sensor",
        "reload":"Neu Laden",
        "restartMcu":"MCU Neustart",
        "save":"Speichern",
        "hide":"Verstecke",
        "msg_waitForMcuRestart":"MCU wird neu gestartet...",
        "msg_enterNewName":"Neuer Name:",
        "msg_firmwareIsUpdating":"Firmware aktualisiert sich...",
    }
} ;

function setupI18n({ withLocale: _locale } = { withLocale: 'en' }) {
    addMessages('en', en);
    addMessages('de', de);

    init({
        fallbackLocale: 'en',
        initialLocale: getLocaleFromNavigator(),
      });
}


export { _, setupI18n };

