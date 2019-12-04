/**
 * XinaBox SL06 extension for makecode
 *
 */

/**
* SL06 block
*/
//% color=#444444 icon="\uf0eb"
//% groups=[Colour, Light, Gesture, Proximity, Optional]
namespace SL06 {
    //%blockId=SL06_begin
    //%block="SL06 begin"
    //%advanced=true
    //%group=Optional
    //%shim=SL06::begin
    export function begin(): void {
        return;
    }

    //%blockId=SL06_getMode
    //%block="SL06 get mode"
    //%advanced=true
    //%group=Optional
    //%shim=SL06::getMode
    export function getMode(): number {
        return 1;
    }

    //%blockId=SL06_setMode
    //%block="SL06 set mode"
    //%advanced=true
    //%group=Optional
    //%shim=SL06::setMode
    export function setMode(mode: number, enable: number): void {
        return;
    }

    //%blockId=SL06_enablePower
    //%block="SL06 enable power"
    //%group=Optional
    //%shim=SL06::enablePower
    export function enablePower(): void {
        return;
    }

    //%blockId=SL06_disablePower
    //%block="SL06 disable power"
    //%group=Optional
    //%shim=SL06::disablePower
    export function disbalePower(): void {
        return;
    }

    //%blockId=SL06_enableGestureSensor
    //%block="SL06 enable gesture sensor"
    //%group=Gesture
    //%shim=SL06::enableGestureSensor
    export function enableGestureSensor(interrupts: boolean = false): void {
        return;
    }

    //%blockId=SL06_disableGestureSensor
    //%block="SL06 disable gesture sensor"
    //%group=Gesture
    //%shim=SL06::disableGestureSensor
    export function disableGestureSensor(): void {
        return;
    }

    //%blockId=SL06_getLEDDRiver
    //%block="SL06 get LED driver"
    //%advanced=true
    //%group=Optional
    //%shim=SL06::getLEDDRiver
    export function getLEDDRiver(): void {
        return;
    }

    //%blockId=SL06_setLEDDRiver
    //%block="SL06 set LED driver"
    //%advanced=true
    //%group=Optional
    //%shim=SL06::setLEDDRiver
    export function setLEDDRiver(drive: number): void {
        return;
    }

    //%blockId=SL06_getGestureLEDDrive
    //%block="SL06 get gesture LED drive"
    //%advanced=true
    //%group=Gesture
    //%shim=SL06::getGestureLEDDrive
    export function getGestureLEDDrive(drive: number): void {
        return;
    }

    //%blockId=SL06_setGestureLEDDrive
    //%block="SL06 set gesture LED drive"
    //%group=Gesture
    //%advanced=true
    //%shim=SL06::setGestureLEDDrive
    export function setGestureLEDDrive(): void {
        return;
    }

    //%blockId=SL06_getGestureGain
    //%block="SL06 get gesture gain"
    //%advanced=true
    //%group=Gesture
    //%shim=SL06::getGestureGain
    export function getGestureGain(): void {
        return;
    }

    //%blockId=SL06_setGestureGain
    //%block="SL06 set gesture gain"
    //%advanced=true
    //%group=Gesture
    //%shim=SL06::setGestureGain
    export function setGestureGain(gain: number): void {
        return;
    }

    //%blockId=SL06_getGestureIntEnable
    //%block="SL06 get gesture int enable"
    //%advanced=true
    //%group=Gesture
    //%shim=SL06::getGestureIntEnable
    export function getGestureIntEnable(): void {
        return;
    }

    //%blockId=SL06_setGestureIntEnable
    //%block="SL06 set gesture int enable"
    //%group=Gesture
    //%advanced=true
    //%shim=SL06::setGestureIntEnable
    export function setGestureIntEnable(enable: number): void {
        return;
    }

    //%blockId=SL06_isGestureAvailable
    //%block="SL06 is gesture available"
    //%group=Gesture
    //%shim=SL06::isGestureAvailable
    export function isGestureAvailable(): boolean {
        return true;
    }

    //%blockId=SL06_getGesture
    //%block="SL06 get gesture"
    //%group=Gesture
    //%shim=SL06::getGesture
    export function getGesture(): number {
        return 1;
    }

    //%blockId=SL06_enableProximitySensor
    //%block="SL06 enable proximity sensor"
    //%group=Proximity
    //%shim=SL06::enableProximitySensor
    export function enableProximitySensor(interrupts: boolean): void {
        return;
    }

    //%blockId=SL06_disableProximitySensor
    //%block="SL06 disble proximity sensor"
    //%group=Proximity
    //%shim=SL06::disableProximitySensor
    export function disableProximitySensor(): void {
        return;
    }

    //%blockId=SL06_getProximityGain
    //%block="SL06 get proximity gain"
    //%advanced=true
    //%group=Proximity
    //%shim=SL06::getProximityGain
    export function getProximityGain(): number {
        return 1;
    }

    //%blockId=SL06_setProximityGain
    //%block="SL06 set proximity gain"
    //%advanced=true
    //%group=Proximity
    //%shim=SL06::setProximityGain
    export function setProximityGain(): void {
        return;
    }

    //%blockId=SL06_getProximity
    //%block="SL06 get proximity"
    //%group=Proximity
    //%shim=SL06::getProximity
    export function getProximity(): number {
        return 1;
    }

    //%blockId=SL06_enableLightSensor
    //%block="SL06 enable light sensor"
    //%group=Light
    //%shim=SL06::enableLightSensor
    export function enableLightSensor(intterupts: boolean = false): void {
        return;
    }

    //%blockId=SL06_disableLightSensor
    //%block="SL06 disable light sensor"
    //%group=Light
    //%shim=SL06::disableLightSensor
    export function disableLightSensor(): void {
        return;
    }

    //%blockId=SL06_getAmbientLightGain
    //%block="SL06 get ambient light gain"
    //%group=Light
    //%advanced=true
    //%shim=SL06::getAmbientLightGain
    export function getAmbientLightGain(): number {
        return 1;
    }

    //%blockId=SL06_setAmbientLightGain
    //%block="SL06 set ambient light gain"
    //%group=Light
    //%advanced=true
    //%shim=SL06::setAmbientLightGain
    export function setAmbientLightGain(gain: number): void {
        return
    }

    //%blockId=SL06_clearAmbientLightInt
    //%block="SL06 clear ambient light int"
    //%group=Light
    //%advanced=true
    //%shim=SL06::clearAmbientLightInt
    export function clearAmbientLightInt(): void {
        return;
    }

    //%blockId=SL06_getAmbientLight
    //%block="SL06 get ambient light"
    //%group=Light
    //%shim=SL06::getAmbientLight
    export function getAmbientLight(): number {
        return 1
    }

    //%blockId=SL06_getRedLight
    //%block="SL06 get red light"
    //%group=Light
    //%shim=SL06::getRedLight
    export function getRedLight(): number {
        return 1
    }

    //%blockId=SL06_getGreenLight
    //%block="SL06 get green light"
    //%group=Light
    //%shim=SL06::getGreenLight
    export function getGreenLight(): number {
        return 1
    }

    //%blockId=SL06_getBlueLight
    //%block="SL06 get blue light"
    //%group=Light
    //%shim=SL06::getBlueLight
    export function getBlueLight(): number {
        return 1;
    }

    begin();
}
