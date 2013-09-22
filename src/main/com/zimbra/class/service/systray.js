/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Zimbra Mail Notifier.
 *
 * The Initial Developer of the Original Code is
 * David GUEHENNEC.
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

"use strict";

// import js-ctypes
Components.utils.import("resource://gre/modules/ctypes.jsm");

if (!com) {
    var com = {};
}
if (!com.zimbra) {
    com.zimbra = {};
}
if (!com.zimbra.service) {
    com.zimbra.service = {};
}

/**
 * Creates an instance of the systray.
 *
 * @constructor
 * @this {Systray}
 * @param {Service}
 *           The parent object
 */
com.zimbra.service.Systray = function(serv) {

    this._logger = new com.zimbra.service.Logger("Systray");
    this._service = serv;

    // The library
    this._lib = null;

    // The functions
    this._func_init = null;
    this._func_setUnreadMail = null;
    this._func_unload = null;

    // Error to print if failed
    var errMsg = "";

    // Open the windows library
    if (this._lib === null) {
        try {
            var winLib = this._dirLib().clone();
            winLib.append("libzimbrasystray.dll");
            if (!winLib.exists()) {
                throw new Error("Not found: libzimbrasystray.dll");
            }
            this._lib = ctypes.open(winLib.path);
        }
        catch (e) {
            errMsg += " [Windows: " + e + "]";
        }
    }

    // Open the unix library
    if (this._lib === null) {
        try {
            var unixLib = this._dirLib().clone();
            unixLib.append("libzimbrasystray.so");
            if (!unixLib.exists()) {
                throw new Error("Not found: libzimbrasystray.so");
            }
            this._lib = ctypes.open(unixLib.path);
        }
        catch (e) {
            errMsg += " [Unix: " + e + "]";
        }
    }


    if (this._lib === null) {
        this._logger.error("Fail to open lib:" + errMsg);
    }
    else {
        // Get the functions pointers
        try {
            this._func_init = this._lib.declare("ZIMBRA_SYSTRAY_init",
                                                ctypes.default_abi,
                                                ctypes.int32_t);

            this._func_unload = this._lib.declare("ZIMBRA_SYSTRAY_unload",
                                                ctypes.default_abi,
                                                ctypes.int32_t);

            this._func_setConnected = this._lib.declare("ZIMBRA_SYSTRAY_setConnected",
                                                        ctypes.default_abi,
                                                        ctypes.void_t,
                                                        ctypes.int32_t);

            this._func_setUnreadMail = this._lib.declare("ZIMBRA_SYSTRAY_setUnreadMail",
                                                        ctypes.default_abi,
                                                        ctypes.void_t,
                                                        ctypes.int32_t);
        }
        catch (e) {
            this._logger.error("Fail to declare function lib: " + e);
        }
    }
};

/**
 * Get the directory containing the library
 *
 * @this {Systray}
 * @return {nsIFile}
 */
com.zimbra.service.Systray.prototype._dirLib = function() {

    var resServ = Services.io.getProtocolHandler("resource")
                    .QueryInterface(Components.interfaces.nsIResProtocolHandler);
    var iOServ = Components.classes["@mozilla.org/network/io-service;1"]
                    .getService(Components.interfaces.nsIIOService);

    var uLib = iOServ.newURI("resource://zimbra_mail_notifier_lib/", null, null);
    uLib = iOServ.newURI(resServ.resolveURI(uLib), null, null);

    if (uLib instanceof Components.interfaces.nsIFileURL) {
        return uLib.file;
    }
    throw new Error("not resolved");
};

/**
 * Init the systray
 *
 * @this {Systray}
 * @return {Boolean} true if success
 */
com.zimbra.service.Systray.prototype.init = function() {
    try {
        var f_init = this._func_init;
        if (f_init) {
            var res = f_init();
            if (res !== 0) {
                this._logger.error("Fail to run init, code: " + res);
            }
            else {
                return true;
            }
        }
    }
    catch (e) {
        this._logger.error("Fail to run init: " + e);
    }
    return false;
};

/**
 * Init the systray
 *
 * @this {Systray}
 * @return {Boolean} true if success
 */
com.zimbra.service.Systray.prototype.unload = function() {
    try {
        var f_unload = this._func_unload;
        if (f_unload) {
            var res = f_unload();
            if (res !== 0) {
                this._logger.error("Fail to run unload, code: " + res);
            }
            else {
                return true;
            }
        }
    }
    catch (e) {
        this._logger.error("Fail to run unload: " + e);
    }
    return false;
};

/**
 * Inform if we are connected, should only be run when the status changed
 *
 * @this {Systray}
 * @param {Boolean}
 *           isConnected
 */
com.zimbra.service.Systray.prototype.setConnected = function(isConnected) {
    try {
        var f_setco = this._func_setConnected;
        if (f_setco) {
            f_setco(isConnected ? 1 : 0);
        }
    }
    catch (e) {
        this._logger.error("Fail to run setConnected: " + e);
    }
};

/**
 * Set the number of unread mail
 *
 * @this {Systray}
 * @param {Number}
 *           nb  The number of unread mail
 */
com.zimbra.service.Systray.prototype.setUnreadMail = function(nb) {
    try {
        var f_unreadmail = this._func_setUnreadMail;
        if (f_unreadmail) {
            f_unreadmail(nb);
        }
    }
    catch (e) {
        this._logger.error("Fail to run setUnreadMail: " + e);
    }
};

/**
 * refresh interface.
 *
 * @this {Main}
 */
com.zimbra.service.Systray.prototype.refresh = function(startRequest) {
    if (!startRequest && this._service) {
        if (this._service.isConnected()) {
            var nbUnreadMessages = this._service.getNbMessageUnread();
            this.setUnreadMail(nbUnreadMessages);
        }
        else {
            this.setConnected(false);
        }
    }
}
