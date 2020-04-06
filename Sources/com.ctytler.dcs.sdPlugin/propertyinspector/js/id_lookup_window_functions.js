
function loaded() {
    RequestInstalledModules();
    setTimeout(() => {
        // this is just a quick message to Property Inspector (you should see it in the console)
        window.opener.gotCallbackFromWindow("Message from window: " + new Date().toLocaleTimeString());
    }, 1000);

};


/** Functions for communication with original window **/

function sendmessage(event, payload) {
    // Calls function from original (Property Inspector) window.
    var msg = { "event": event }
    if (payload) {
        msg["payload"] = payload;
    }
    window.opener.gotCallbackFromIdLookupWindow(msg);
}




function RequestInstalledModules() {
    var dcs_install_path = document.getElementById("dcs_install_path").value;
    sendmessage("RequestInstalledModules", dcs_install_path);
    const settings = { "dcs_install_path": dcs_install_path };
    sendmessage("UpdateGlobalSettings", settings);
}

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
    callbackRequestIdLookup();
}

function modifyInstalledModulesList(installed_modules_list) {
    modified_list = [];
    if (installed_modules_list.length > 0) {
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

function callbackRequestIdLookup() {
    clearTableContents()
    var select_elem = document.getElementById("select_module");
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



/**  Table Handling Functions **/

function clearTableContents() {
    document.getElementById("clickabledata_table_search").value = "";
    document.getElementById("import_selection_div").hidden = true;
    var tbody = document.getElementById("clickabledata_table").getElementsByTagName("tbody")[0];
    while (tbody.getElementsByTagName("tr").length > 0) {
        tbody.deleteRow(0);
    }
    selected_row = "";
}

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


/** ID Import Functions **/

function callbackImportDcsCommandAndImageChange() {
    callbackImportDcsCommand();
    callbackImportImageChange();
}

function callbackImportDcsCommand() {
    var td = selected_row.getElementsByTagName("td");
    var device_str = td[0].textContent;
    var device_id = device_str.split("(").pop().split(")")[0];
    payload = {
        "device_id": device_id,
        "button_id": td[1].textContent,
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
