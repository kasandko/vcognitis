import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import "../../SimpleControls"
import "../../SpecialControls"
import "../../Singletons"

import ConfederateFinder 1.0

Drawer {
    id: drawer

    property int sex: 2
    property alias filterModels: filterModels

    function indexOf(model, id) {
        for (var i = 0; i < model.count; i++) {
            if (model.get(i).value === id) {
                return i;
            }
        }

        return CFConstants.invalidIndex
    }

    function getCurrentCountryId() {
        if (countryComboBox.currentIndex == -1)
            return -1;
        return userManager.countryProxyModel.data(userManager.countryProxyModel.indexByRow(countryComboBox.currentIndex), CFCountryRoles.Id);
    }

    function reset() {
        countryComboBox.currentIndex = userManager.countryProxyModel.find(userManager.model.country);
        citySelector.reset(userManager.model.city);
        sex = userManager.model.sex;
        photoCheckbox.checked = userManager.model.photo;
        religionComboBox.reset(userManager.model.religion);
        relationComboBox.reset(userManager.model.relation);
        politcalComboBox.reset(userManager.model.political);
        lifeMainComboBox.reset(userManager.model.lifeMain);
        peopleMainComboBox.reset(userManager.model.peopleMain);
        smokingComboBox.reset(userManager.model.smoking);
        alcoholComboBox.reset(userManager.model.alcohol);
        downloadedGroupSelector.reset(userManager.model.groups);
        limitTextField.text = userManager.model.limit;
        searchAgeCheckBox.checked = userManager.model.searchAge;
        exactCheckbox.checked = userManager.model.exact;

        var age = userManager.model.fromAge;
        if (age) {
            ageSwitch.checked = true;
            toAgeSpinBox.value = 200;
            fromAgeSpinBox.value = age;
            toAgeSpinBox.value = userManager.model.toAge;
        } else {
            ageSwitch.checked = false;
        }

        statisticsColumn.clear();
    }

    function update() {
        userManager.model.country = countryComboBox.currentIndex == -1 ? 0 : userManager.countryProxyModel.data(userManager.countryProxyModel.indexByRow(countryComboBox.currentIndex), CFCountryRoles.Id);
        userManager.model.city = citySelector.cityItems();
        userManager.model.sex = sex;
        userManager.model.photo = photoCheckbox.checked;
        userManager.model.political = politcalComboBox.uids();
        userManager.model.religion = religionComboBox.uids();
        userManager.model.relation = relationComboBox.uids();
        userManager.model.lifeMain = lifeMainComboBox.uids();
        userManager.model.peopleMain = peopleMainComboBox.uids();
        userManager.model.smoking = smokingComboBox.uids();
        userManager.model.alcohol = alcoholComboBox.uids();
        userManager.model.groups = downloadedGroupSelector.uids();
        userManager.model.limit = limitTextField.text;
        userManager.model.exact = exactCheckbox.checked;

        if (ageSwitch.checked) {
            userManager.model.fromAge = fromAgeSpinBox.value;
            userManager.model.toAge = toAgeSpinBox.value;
            userManager.model.searchAge = searchAgeCheckBox.checked;
        } else {
            userManager.model.fromAge = 0;
            userManager.model.searchAge = false;
        }

        if (utils.appMode === CFAppMode.Accurate) {
            userManager.startScoring()
        } else {
            if (topAnalyzeCheckBox.checked)
                topAnalyzePopup.open();
            userManager.select();
        }

        close()
    }

    width: 400
    height: parent.height - CFStyle.mainHeaderHeight

    onOpened: reset()
    onSexChanged: statisticsColumn.clear();

    CFFilterModels { id: filterModels }

    Item {
        id: headerItem

        anchors.top: parent.top
        width: parent.width
        height: CFStyle.dialogHeaderHeight

        CFPane {
            anchors.fill: parent

            Item {

                anchors.fill: parent

                CFLabel {
                    text: qsTr("Параметры поиска")

                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 18
                }

                CFImage {
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        rightMargin: 40
                    }

                    source: "qrc:/icons/Icon_like_full.svg"
                    sourceSize: Qt.size(24, 24)

                    // On click effect
                    opacity: likeMouseArea.containsMouse ? (likeMouseArea.containsPress ? 0.8: 0.9) : 1

                    MouseArea {
                        id: likeMouseArea

                        cursorShape: Qt.PointingHandCursor

                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            bottom: parent.bottom
                        }

                        onClicked:  {
                            ageSwitch.checked = true;
                            var age = userManager.currentUser.age ? userManager.currentUser.age : 25;

                            fromAgeSpinBox.value = age - 1;
                            toAgeSpinBox.value = age + 1;

                            sex = userManager.currentUser.sex === 1 ? 2 : 1;

                            relationComboBox.reset([CFPersonalRelation.Searching, CFPersonalRelation.Single])
                        }
                    }
                }

                CFRoundButton {
                    flat: true

                    anchors {
                        right: parent.right
                        rightMargin: -((width - icon.width) / 2)
                        verticalCenter: parent.verticalCenter
                    }

                    icon {
                        source: "qrc:/icons/close.svg"
                        width: 24
                        height: 24
                    }

                    onClicked: close()
                }
            }
        }

        CFHDivider {
            width: parent.width

            anchors {
                bottom: parent.bottom
                bottomMargin: -height
            }
        }
    }

    MouseArea {
        id: hoverMouseArea

        anchors {
            top: headerItem.bottom
            bottom: footerItem.top
            left: parent.left
            right: parent.right
            leftMargin: CFStyle.padding
            rightMargin: leftMargin
        }

        hoverEnabled: true

        Flickable {
            id: filterFlickable

            height: parent.height
            width: parent.width - 1
            clip: true

            boundsBehavior: Flickable.StopAtBounds
            contentHeight: contentColumn.implicitHeight

            HoverHandler {
                id: hoverHandler
            }

            ScrollBar.vertical: CFScrollBar{ targetContainsMouse: hoverMouseArea.containsMouse }

            CFColumn {
                id: contentColumn

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: CFStyle.padding
                bottomPadding: CFStyle.padding
                spacing: CFStyle.spacing

                CFColumn {
                    width: parent.width
                    spacing: CFStyle.padding * 0.75
                    topPadding: CFStyle.padding

                    CFLabel {
                        text: qsTr("Страна")
                        font.weight: Font.Medium
                    }

                    CFSearchComboBox {
                        id: countryComboBox

                        needPlaceholder: countryComboBox.displayText === "" || countryComboBox.displayText === qsTr("Не выбрано")
                        placeholder: qsTr("Выберите страну")
                        width: parent.width
                        model: userManager.countryProxyModel
                        textRole: "name"

                        onCurrentIndexChanged: {
                            statisticsColumn.clear();
                            textRole = "id";
                            var id = parseInt(currentText);
                            textRole = "name";
                            citySelector.clear();
                        }

                        onFilterTextChanged: userManager.countryProxyModel.set_filterString(filterText)
                    }
                }

                CFColumn {
                    visible: !countryComboBox.needPlaceholder
                    spacing: CFStyle.padding * 0.5
                    width: parent.width

                    CFLabel {
                        text: qsTr("Город")
                        font.weight: Font.Medium
                    }

                    CFCitySelector {
                        id: citySelector

                        onContentChanged: statisticsColumn.clear();
                        width: parent.width
                    }
                }

                CFHDivider { width: parent.width }

                RowLayout {
                    width: parent.width
                    spacing: CFStyle.padding

                    Column {
                        Layout.fillWidth: true

                        CFLabel {
                            text: qsTr("Пол")
                            font.weight: Font.Medium
                        }

                        Column {
                            width: parent.width

                            CFRadioButton {
                                width: parent.width
                                text: qsTr("Женский")
                                checked: sex === 1
                                onClicked: sex = 1;
                            }

                            CFRadioButton {
                                width: parent.width
                                text: qsTr("Мужской")
                                checked: sex === 2
                                onClicked: sex = 2
                            }

                            CFRadioButton {
                                width: parent.width
                                text: qsTr("Любой")
                                checked: sex === 3
                                onClicked: sex = 3
                            }
                        }
                    }

                    CFVDivider {
                        Layout.fillHeight: true
                    }

                    Column {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width / 2 - CFStyle.padding

                        spacing: CFStyle.spacing * 0.75

                        Item {
                            implicitHeight: ageLabel.implicitHeight
                            implicitWidth: ageLabel.implicitWidth + ageSwitch.implicitWidth + ageSwitch.anchors.leftMargin

                            CFLabel {
                                id: ageLabel

                                font.weight: Font.Medium
                                text: qsTr("Возраст")
                            }

                            CFSwitch {
                                id: ageSwitch

                                onCheckedChanged: statisticsColumn.clear();
                                anchors.verticalCenter: ageLabel.verticalCenter
                                anchors.left: ageLabel.right
                                anchors.leftMargin: CFStyle.spacing * 0.5
                            }
                        }

                        CFRow {
                            spacing: CFStyle.spacing / 2

                            CFLabel {
                                width: 20
                                text: qsTr("от")
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            CFSpinBox {
                                id: fromAgeSpinBox

                                onValueChanged: statisticsColumn.clear();
                                value: 16
                                from: 16
                                to: toAgeSpinBox.value
                                enabled: ageSwitch.checked
                            }
                        }

                        CFRow {
                            spacing: CFStyle.spacing / 2

                            CFLabel {
                                width: 20
                                text: qsTr("до")
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            CFSpinBox {
                                id: toAgeSpinBox

                                onValueChanged: statisticsColumn.clear();
                                value: 25
                                from: fromAgeSpinBox.value
                                enabled: ageSwitch.checked
                            }
                        }

                        CFCheckBox {
                            id: searchAgeCheckBox

                            text: qsTr("Уточнять возраст")
                            hoverEnabled: true
                            enabled: ageSwitch.checked

                            CFToolTip {
                                visible: parent.hovered
                                text: qsTr("Может сильно замедлить работу, при\nбольшом кол-ве загруженных групп")
                            }
                        }
                    }
                }

                CFHDivider { width: parent.width }

                CFColumn {
                    width: parent.width
                    spacing: CFStyle.padding * 0.5

                    CFMultiComboBox {
                        id: relationComboBox

                        width: parent.width
                        model: filterModels.relationModel
                        textRole: "name"
                        placeholder: qsTr("Семейное положение")
                        hintPlaceHodler: qsTr("Выберите статус")

                        onContentChanged: statisticsColumn.clear();
                    }

                    CFCheckBox {
                        id: photoCheckbox

                        onCheckedChanged: statisticsColumn.clear();
                        text: qsTr("С фотографией")
                    }

                    CFHDivider { width: parent.width }
                }


                CFMultiComboBox {
                    id: peopleMainComboBox

                    width: parent.width
                    model: filterModels.peopleMainModel
                    textRole: "name"
                    placeholder: qsTr("Главное в людях")
                    onContentChanged: statisticsColumn.clear();
                }

                CFMultiComboBox {
                    id: religionComboBox

                    width: parent.width
                    model: filterModels.religionModel
                    textRole: "name"
                    placeholder: qsTr("Мировоззрение")
                    onContentChanged: statisticsColumn.clear();
                }


                CFMultiComboBox {
                    id: politcalComboBox

                    width: parent.width
                    model: filterModels.politicalModel
                    textRole: "name"
                    placeholder: qsTr("Политические взгляды")
                    onContentChanged: statisticsColumn.clear();
                }

                CFMultiComboBox {
                    id: lifeMainComboBox

                    width: parent.width
                    model: filterModels.lifeMainModel
                    textRole: "name"
                    placeholder: qsTr("Главное в жизни")
                    onContentChanged: statisticsColumn.clear();
                }

                CFMultiComboBox {
                    id: smokingComboBox

                    width: parent.width
                    model: filterModels.smokingModel
                    textRole: "name"
                    placeholder: qsTr("Отношение к курению")
                    onContentChanged: statisticsColumn.clear();
                }

                CFMultiComboBox {
                    id: alcoholComboBox

                    width: parent.width
                    model: filterModels.alcoholModel
                    textRole: "name"
                    placeholder: qsTr("Отношение к алкоголю")
                    onContentChanged: statisticsColumn.clear();
                }

                CFCheckBox {
                    id: exactCheckbox

                    onCheckedChanged: statisticsColumn.clear();
                    text: qsTr("Строгое соответствие всем фильтрам")
                }

                CFHDivider { width: parent.width }

                CFColumn {
                    spacing: CFStyle.padding * 0.5
                    width: parent.width

                    CFLabel {
                        text: qsTr("Искать только в")
                        font.weight: Font.Medium
                    }

                    CFDownloadedGroupSelector {
                        id: downloadedGroupSelector

                        width: parent.width

                        onContentChanged: statisticsColumn.clear();
                    }
                }

                CFHDivider { width: parent.width }

                CFCheckBox {
                    id: topAnalyzeCheckBox

                    visible: utils.appMode == CFAppMode.Quick

                    checked: true
                    text: qsTr("Предлагать уточнение результата")
                }

                CFColumn {
                    visible: utils.appMode == CFAppMode.Accurate

                    spacing: CFStyle.padding * 0.5
                    width: parent.width

                    CFLabel {
                        text: qsTr("Количество загружаемых профилей")
                        font.weight: Font.Medium
                    }

                    CFTextField {
                        id: limitTextField

                        error: text > 100000

                        validator: IntValidator {
                            bottom: 0
                            top: 100000
                        }
                        horizontalAlignment: TextInput.AlignHCenter
                    }
                }

                CFHDivider { width: parent.width }

                CFColumn {
                    id: statisticsColumn

                    property int userByFilter: -1
                    property int userByFilterScored: -1

                    function clear() {
                        userByFilter = -1;
                        userByFilterScored = -1;
                    }

                    spacing: CFStyle.spacing * 0.75

                    CFLabel {
                        visible: statisticsColumn.userByFilter == -1

                        text: qsTr("Узнать статистику по этому набору фильтров")
                        color: CFStyle.blueMain

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -1

                            hoverEnabled: true
                            onContainsMouseChanged: parent.opacity = containsMouse ? 0.8 : 1

                            onClicked: {
                                statisticsColumn.userByFilter = userManager.model.usersCountByFilterSettings(
                                            countryComboBox.currentIndex == -1 ? 0 : userManager.countryProxyModel.data(userManager.countryProxyModel.indexByRow(countryComboBox.currentIndex), CFCountryRoles.Id),
                                            sex,
                                            citySelector.cityItems(),
                                            photoCheckbox.checked,
                                            ageSwitch.checked ? fromAgeSpinBox.value : 0,
                                            toAgeSpinBox.value,
                                            relationComboBox.uids(),
                                            downloadedGroupSelector.uids(),
                                            exactCheckbox.checked,
                                            politcalComboBox.uids(),
                                            religionComboBox.uids(),
                                            lifeMainComboBox.uids(),
                                            peopleMainComboBox.uids(),
                                            smokingComboBox.uids(),
                                            alcoholComboBox.uids());

                                statisticsColumn.userByFilterScored = userManager.model.usersCountByFilterSettingsScored(
                                            countryComboBox.currentIndex == -1 ? 0 : userManager.countryProxyModel.data(userManager.countryProxyModel.indexByRow(countryComboBox.currentIndex), CFCountryRoles.Id),
                                            sex,
                                            citySelector.cityItems(),
                                            photoCheckbox.checked,
                                            ageSwitch.checked ? fromAgeSpinBox.value : 0,
                                            toAgeSpinBox.value,
                                            relationComboBox.uids(),
                                            downloadedGroupSelector.uids(),
                                            exactCheckbox.checked,
                                            politcalComboBox.uids(),
                                            religionComboBox.uids(),
                                            lifeMainComboBox.uids(),
                                            peopleMainComboBox.uids(),
                                            smokingComboBox.uids(),
                                            alcoholComboBox.uids());

                                filterFlickable.contentY = 9999; // hack for showing full statiscits
                            }
                        }
                    }

                    CFLabel {
                        visible: statisticsColumn.userByFilter != -1

                        text: qsTr("<b><font color=\"#000000\">%1</font></b> запис(ей) в базе", "",
                                   userManager.model.usersDownloaded >= 1000 ? 5/*any plural*/: userManager.model.usersDownloaded)
                        .arg(utils.subsToString(userManager.model.usersDownloaded))
                    }

                        CFLabel {
                            visible: statisticsColumn.userByFilter != -1

                            text: qsTr("<b><font color=\"#000000\">%1 чел.</font></b> соответствующих фильтру", "",
                                       statisticsColumn.userByFilter >= 1000 ? 5/*any plural*/:statisticsColumn.userByFilter)
                            .arg(utils.subsToString(statisticsColumn.userByFilter))
                        }
                        CFLabel {
                            visible: statisticsColumn.userByFilterScored != -1 && (utils.appMode == CFAppMode.Accurate || topAnalyzeCheckBox.checked)

                            text: qsTr("<b><font color=\"#000000\">%1 чел.</font></b> проанализированно", "",
                                       statisticsColumn.userByFilterScored >= 1000 ? 5/*any plural*/:statisticsColumn.userByFilterScored)
                            .arg(utils.subsToString(statisticsColumn.userByFilterScored))
                        }
                        }
                        }
                        }
                        }

                            Item {
                                id: footerItem

                                anchors.bottom: parent.bottom
                                implicitHeight: footerButtonRow.implicitHeight + CFStyle.padding * 2
                                width: parent.width

                                Row {
                                    id: footerButtonRow

                                    spacing: CFStyle.spacing

                                    anchors {
                                        right: parent.right
                                        left: parent.left
                                        margins: CFStyle.padding
                                        verticalCenter: parent.verticalCenter
                                    }

                                    CFButton {
                                        id: cancelButton

                                        text: qsTr("Отмена")
                                        width: (parent.width - CFStyle.padding) / 2
                                        Material.foreground: CFStyle.blue
                                        Material.background: CFStyle.blueLight
                                        height: 30
                                        topInset: 0
                                        bottomInset: 0

                                        onClicked: close()
                                    }

                                    CFButton {
                                        id: saveButton

                                        enabled: !userManager.downloadQueue.count && !limitTextField.error
                                        text: qsTr("Применить фильтры")
                                        width: (parent.width - CFStyle.padding) / 2

                                        height: 30
                                        topInset: 0
                                        bottomInset: 0

                                        onClicked: drawer.update()
                                    }
                                }

                                CFHDivider {
                                    width: parent.width

                                    anchors {
                                        top: parent.top
                                    }
                                }
                            }
                        }
