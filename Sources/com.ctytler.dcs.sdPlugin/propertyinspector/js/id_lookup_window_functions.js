// Copyright 2020, Charles Tytler

/**
 * Called when window is opened.
 */
function loaded() {
    restoreGlobalSettings(window.opener.global_settings);
};


/**
 * Restores previously set global settings saved from a previous Streamdeck context.
 * 
 * @param settings Global settings json stored by Streamdeck program
 */
function restoreGlobalSettings(settings) {
    document.getElementById("dcs_install_path").value = settings.dcs_install_path;
    RequestInstalledModules();
    console.log("Restored global settings: ", settings);
}

function UpdateGlobalSettings() {
    window.opener.global_settings["dcs_install_path"] = document.getElementById("dcs_install_path").value;
    let select_elem = document.getElementById("select_module");
    let selected_module = ""
    if (select_elem.options.length > 0) {
        selected_module = select_elem.options[select_elem.selectedIndex].value;
        window.opener.global_settings["last_selected_module"] = selected_module;
    }
    sendmessage("UpdateGlobalSettings", window.opener.global_settings);
}


/**
 * Requests from the DCS Interface plugin the list of installed aircraft modules in the DCS Install Path.
 */
function RequestInstalledModules() {
    var dcs_install_path = document.getElementById("dcs_install_path").value;
    sendmessage("RequestInstalledModules", dcs_install_path);
    UpdateGlobalSettings();
}

/**
 * Function that populates the module drop-down selection when the list of installed modules are returned by the plugin.
 * 
 * @param {Json} installed_modules_list 
 */
function gotInstalledModules(installed_modules_list) {
    var select_elem = document.getElementById("select_module");
    // Remove any previously listed modules from dropdown.
    while (select_elem.options.length > 0) {
        select_elem.remove(0);
    }

    // Populate dropdown with received modules with modification.
    modified_modules_list = modifyInstalledModulesList(installed_modules_list);
    for (module of modified_modules_list) {
        var option = document.createElement('option');
        option.appendChild(document.createTextNode(module));
        option.value = module;
        select_elem.append(option);
    }

    // Select the last selected module for convenience.
    setSelectedModule(window.opener.global_settings.last_selected_module);
    callbackRequestIdLookup();
}

/**
 * Applies modifications to the module list for special cases (such as modules with multiple versions)
 * 
 * @param {Json} installed_modules_list 
 */
function modifyInstalledModulesList(installed_modules_list) {
    modified_list = [];
    if (installed_modules_list != null) {
        modified_list.push("General"); // Make "General" the first module listed.
        for ([idx, module] of installed_modules_list.entries()) {
            if (module == "L-39C") {
                installed_modules_list.push("L-39ZA")
            }
            if (module == "C-101") {
                installed_modules_list[idx] = "C-101CC";
                installed_modules_list.push("C-101EB");
            }
        }
        installed_modules_list.sort();
        modified_list = modified_list.concat(installed_modules_list);
    }
    return modified_list;
}

/**
 * Requests list of DCS ID attributes from the plugin for the currently selected module.
 */
function callbackRequestIdLookup() {
    clearTableContents()
    var select_elem = document.getElementById("select_module");
    if (select_elem.options.length > 0) {
        var module = select_elem.options[select_elem.selectedIndex].value;
        if (module == "General" || module == "Flaming Cliffs") {
            // Use the stored general commands from included .js file.
            gotClickabledata(general_commands)
        }
        else {
            var dcs_install_path = document.getElementById("dcs_install_path").value;
            var payload = { "dcs_install_path": dcs_install_path, "module": module };
            sendmessage("RequestIdLookup", payload);
            console.log("Request ID Lookup for: ", payload);
        }
    }
    UpdateGlobalSettings();
}


/**
 * Sets the drop-down "select_module" to a specific module name.
 * 
 * @param {string} module 
 */
function setSelectedModule(module) {
    if (module != "") {
        var select_elem = document.getElementById("select_module");
        for (var i = 0; i < select_elem.options.length; i++) {
            if (select_elem.options[i].value == module) {
                select_elem.selectedIndex = i;
                break;
            }
        }
    }
}


/**  Table Handling Functions **/

/**
 * Clears all rows of table.
 */
function clearTableContents() {
    document.getElementById("clickabledata_table_search").value = "";
    document.getElementById("import_selection_div").hidden = true;
    var tbody = document.getElementById("clickabledata_table").getElementsByTagName("tbody")[0];
    while (tbody.getElementsByTagName("tr").length > 0) {
        tbody.deleteRow(0);
    }
    selected_row = "";
}

/**
 * Populates table with clickabledata elements in each row, sorted by column values.
 * 
 * @param {Json} clickabledata_elements 
 */
function gotClickabledata(clickabledata_elements) {
    // Create rows in a new table body so it is easy to replace any old content.
    var new_table_body = document.createElement('tbody');
    clickabledata_elements.sort();
    for (const element of clickabledata_elements) {
        var split_elem = element.split(',');
        var new_row = new_table_body.insertRow();
        new_row.insertCell(0).appendChild(document.createTextNode(split_elem[0])); // Device
        new_row.insertCell(1).appendChild(document.createTextNode(split_elem[1])); // Button ID
        new_row.insertCell(2).appendChild(document.createTextNode(split_elem[2])); // Element
        new_row.insertCell(3).appendChild(document.createTextNode(split_elem[3])); // Type
        new_row.insertCell(4).appendChild(document.createTextNode(split_elem[4])); // DCS ID
        new_row.insertCell(5).appendChild(document.createTextNode(split_elem[5])); // Click Value
        new_row.insertCell(6).appendChild(document.createTextNode(split_elem[6])); // Limit Min
        new_row.insertCell(7).appendChild(document.createTextNode(split_elem[7])); // Limit Max
        new_row.insertCell(8).appendChild(document.createTextNode(split_elem[8])); // Description
        new_row.addEventListener('click', function () { selectRow(this) });
    }
    var document_table_body = document.getElementById("clickabledata_table").getElementsByTagName('tbody')[0];
    document_table_body.parentNode.replaceChild(new_table_body, document_table_body);
}

/**
 * Call back function that shows/hides table rows if they do/don't contain the search query.
 */
function searchTable() {
    var query = document.getElementById("clickabledata_table_search").value.toUpperCase();
    var table = document.getElementById("clickabledata_table");
    var body = table.getElementsByTagName("tbody")[0];
    var tr = body.getElementsByTagName("tr");
    for (var i = 0; i < tr.length; i++) {
        var row_text = "";
        var td = tr[i].getElementsByTagName("td");
        for (j = 0; j < td.length; j++) {
            row_text += td[j].textContent;
        }
        if (row_text.toUpperCase().indexOf(query) >= 0) {
            tr[i].style.display = "";
        }
        else {
            tr[i].style.display = "none";
        }
    }
}

/**
 * Function that provides highlighting and managing of currently selected row.
 */
var selected_row = '';
function selectRow(elem) {
    if (selected_row != '') { selected_row.classList.toggle("highlighted"); };
    if (selected_row == elem) {
        selected_row = '';
        document.getElementById("import_selection_div").hidden = true;
    }
    else {
        elem.classList.toggle("highlighted");
        document.getElementById("import_selection_div").hidden = false;
        selected_row = elem;
    }
}


/** ID Import Button onClick Functions **/

function callbackImportDcsCommandAndImageChange() {
    callbackImportDcsCommand();
    callbackImportImageChange();
}

/**
 * Imports ID elements for all button type command settings.
 */
function callbackImportDcsCommand() {
    var td = selected_row.getElementsByTagName("td");
    var device_str = td[0].textContent;
    var device_id = device_str.split("(").pop().split(")")[0];
    payload = {
        "device_id": device_id,
        "button_id": td[1].textContent,
        "dcs_id": td[4].textContent,
        "click_value": td[5].textContent,
        "limit_min": td[6].textContent,
        "limit_max": td[7].textContent
    };
    sendmessage("ImportDcsCommand", payload);
}

function callbackImportImageChange() {
    var td = selected_row.getElementsByTagName("td");
    payload = {
        "dcs_id": td[4].textContent
    };
    sendmessage("ImportImageChange", payload);
}

function callbackImportTextChange() {
    var td = selected_row.getElementsByTagName("td");
    payload = {
        "dcs_id": td[4].textContent
    };
    sendmessage("ImportTextChange", payload);
}



/** Functions for communication with original window **/


/**
 * Sends a message to the DCS Interface plugin with the "event" type and a json payload.
 * 
 * @param {string} event 
 * @param {json} payload 
 */
function sendmessage(event, payload) {
    // Calls function from original (Property Inspector) window.
    var msg = { "event": event }
    if (payload) {
        msg["payload"] = payload;
    }
    window.opener.gotCallbackFromIdLookupWindow(msg);
}
