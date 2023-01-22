/**
 * @file scalevoid.h
 *
 * @brief
 */

#if (BREWMODE == 2 || ONLYPIDSCALE == 1)
/**
 * @brief Check measured weight
 */
void checkWeight() {
    static boolean newDataReady = 0;
    unsigned long currentMillisScale = millis();

    if (scaleFailure) {   // abort if scale is not working
        return;
    }

    if (currentMillisScale - previousMillisScale >= intervalWeight) {
        previousMillisScale = currentMillisScale;

        // check for new data/start next conversion:
        if (LoadCell.update()) {
            newDataReady = true;
        }

        // get smoothed value from the dataset:
        if (newDataReady) {
            weight = LoadCell.getData();
            newDataReady = 0;
        }
    }
}

void displayCountDown(int startValue = 10) {
    while (startValue >= 0 ){
            u8g2.clearBuffer();
            u8g2.drawStr(0, 2, "Calibration in progress.");
            u8g2.drawStr(0, 12, "Place known weight of");
            u8g2.drawStr(0, 22, number2string(scaleKnownWeight));
            u8g2.drawStr(0, 32, "on scale in next");
            u8g2.drawStr(0, 42, number2string(startValue));
            u8g2.sendBuffer();
        startValue--;
        delay(1000);
    }
}

/**
 * @brief Call tare function
*/
void tarescale(boolean calibration=false) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 2, "Taring scale,");
    u8g2.drawStr(0, 12, "remove any load!");
    delay(2000);
    LoadCell.tare();
    if (LoadCell.getTareTimeoutFlag()) {
        debugPrintln("Timeout, check MCU>HX711 wiring and pin designations");
        u8g2.drawStr(0, 32, "failed!");
        u8g2.drawStr(0, 42, "Scale not working...");    // scale timeout will most likely trigger after OTA update, but will still work after boot
        delay(5000);
        u8g2.sendBuffer();
    }
    else {
        if (calibration){
            LoadCell.setCalFactor(1.0);
            LoadCell.tare();
            displayCountDown(10);
            delay(300);
            u8g2.clearBuffer();
            u8g2.drawStr(0, 2, "Calibration running...");
            u8g2.sendBuffer();
            LoadCell.refreshDataSet();
            scaleCalibration = LoadCell.getNewCalibration(scaleKnownWeight);
            LoadCell.setCalFactor(scaleCalibration); // set calibration factor (float)
            writeSysParamsToStorage();
            u8g2.clearBuffer();
            u8g2.drawStr(0, 2, "Calibration done.");
            u8g2.drawStr(0, 12, "New calibration factor:");
            u8g2.drawStr(0, 32, number2string(LoadCell.getCalFactor()));
            u8g2.sendBuffer();
            delay(5000);
        }
        else {
            u8g2.drawStr(0, 42, "done.");
            delay(2000);
            u8g2.sendBuffer();
        }
    }
}
 
/**
 * @brief Initialize scale
 */
void initScale() {
    LoadCell.begin(128);
    long stabilizingtime = 4000; // tare precision can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
    boolean calibration = false;

    u8g2.clearBuffer();
    u8g2.drawStr(0, 2, "Taring scale,");
    u8g2.drawStr(0, 12, "remove any load!");
    
    if((double) scaleCalibration == 1.00){
        u8g2.drawStr(0, 22, "Calibration coming up");
        calibration = true;
    }
    else{
        u8g2.drawStr(0, 22, "....");
    }
    delay(1000);
    u8g2.sendBuffer();
    LoadCell.start(stabilizingtime);
    tarescale(calibration);
    LoadCell.setSamplesInUse(SCALE_SAMPLES);
}

/**
 * @brief Scale with shot timer
 */
void shottimerscale() {
    int tareDone = 0;
    switch (shottimercounter)  {
        case 10:    // waiting step for brew switch turning on
        //Leave time to place a cup under the portafilter and do a tare befor brew
            if (timeBrewed > 0 && < 3000 && tareDone = 0){
                LoadCell.tare();
                tareDone = 1;
            }
            if (timeBrewed >= 6000) {
                weightPreBrew = weight;
                shottimercounter = 20;
            }

            break;

        case 20:
            weightBrew = weight - weightPreBrew;

            if (timeBrewed == 0) {
                shottimercounter = 10;
            }

            break;
    }
}

#endif