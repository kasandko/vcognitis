import QtQuick 2.12
import ConfederateFinder 1.0

Item {
    property alias relationModel: relationModel
    property alias religionModel: religionModel
    property alias politicalModel: politicalModel
    property alias peopleMainModel: peopleMainModel
    property alias lifeMainModel: lifeMainModel
    property alias smokingModel: smokingModel
    property alias alcoholModel: alcoholModel

    function political(value) {
        for (var i = 0; i < politicalModel.count; i++) {
            if (politicalModel.get(i).value === value)
                return politicalModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    function relation(value) {
        for (var i = 0; i < relationModel.count; i++) {
            if (relationModel.get(i).value === value)
                return relationModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    function religion(value) {
        for (var i = 0; i < religionModel.count; i++) {
            if (religionModel.get(i).value === value)
                return religionModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    function lifeMain(value) {
        for (var i = 0; i < lifeMainModel.count; i++) {
            if (lifeMainModel.get(i).value === value)
                return lifeMainModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    function peopleMain(value) {
        for (var i = 0; i < peopleMainModel.count; i++) {
            if (peopleMainModel.get(i).value === value)
                return peopleMainModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    function smoking(value) {
        for (var i = 0; i < smokingModel.count; i++) {
            if (smokingModel.get(i).value === value)
                return smokingModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    function alcohol(value) {
        for (var i = 0; i < alcoholModel.count; i++) {
            if (alcoholModel.get(i).value === value)
                return alcoholModel.get(i).name.toLowerCase()
        }
        return qsTr("[ДАННЫЕ УДАЛЕНЫ]")
    }

    ListModel {
        id: relationModel

        ListElement { value: CFPersonalRelation.Single; name: qsTr("Не женат/Не замужем") }
        ListElement { value: CFPersonalRelation.InRelationship; name: qsTr("Встречаюсь") }
        ListElement { value: CFPersonalRelation.Engaged; name: qsTr("Помолвлен") }
        ListElement { value: CFPersonalRelation.Married; name: qsTr("Женат/Замужем") }
        ListElement { value: CFPersonalRelation.InLove; name: qsTr("Влюблён") }
        ListElement { value: CFPersonalRelation.Complicated; name: qsTr("Все сложно") }
        ListElement { value: CFPersonalRelation.Searching; name: qsTr("В активном поиске") }
        ListElement { value: CFPersonalRelation.CivilUnion; name: qsTr("В гражданском браке") }
    }

    ListModel {
        id: religionModel

        ListElement { value: CFPersonalReligion.Judaism; name: qsTr("Иудаизм") }
        ListElement { value: CFPersonalReligion.Orthodoxy; name: qsTr("Православие") }
        ListElement { value: CFPersonalReligion.Catholicism; name: qsTr("Католичество") }
        ListElement { value: CFPersonalReligion.Protestantism; name: qsTr("Протестанство") }
        ListElement { value: CFPersonalReligion.Islam; name: qsTr("Ислам") }
        ListElement { value: CFPersonalReligion.Buddhism; name: qsTr("Буддизм") }
        ListElement { value: CFPersonalReligion.Confucianism; name: qsTr("Конфуцианство") }
        ListElement { value: CFPersonalReligion.SecularHumanism; name: qsTr("Светский гуманизм") }
        ListElement { value: CFPersonalReligion.Pastafarianism; name: qsTr("Пастафарианство") }
    }

    ListModel {
        id: politicalModel

        ListElement { value: CFPersonalPolitical.Apathetic; name: qsTr("Индифферентные") }
        ListElement { value: CFPersonalPolitical.Communist; name: qsTr("Коммунистические") }
        ListElement { value: CFPersonalPolitical.Socialist; name: qsTr("Социалистические") }
        ListElement { value: CFPersonalPolitical.Moderate; name: qsTr("Умеренные") }
        ListElement { value: CFPersonalPolitical.Liberal; name: qsTr("Либеральные") }
        ListElement { value: CFPersonalPolitical.Conservative; name: qsTr("Консервативные") }
        ListElement { value: CFPersonalPolitical.Monarchist; name: qsTr("Монархические") }
        ListElement { value: CFPersonalPolitical.Ultraconservative; name: qsTr("Ультраконсервативные") }
        ListElement { value: CFPersonalPolitical.Libertian; name: qsTr("Либертарианские") }
    }

    ListModel {
        id: peopleMainModel

        ListElement { value: CFPersonalPeopleMain.IntellectCreativity; name: qsTr("Ум и креативность") }
        ListElement { value: CFPersonalPeopleMain.KindnessHonesty; name: qsTr("Доброта и честность") }
        ListElement { value: CFPersonalPeopleMain.HealthBeauty; name: qsTr("Красота и здоровье") }
        ListElement { value: CFPersonalPeopleMain.WealthPower; name: qsTr("Власть и богатство") }
        ListElement { value: CFPersonalPeopleMain.CouragePersistense; name: qsTr("Смелость и упорство") }
        ListElement { value: CFPersonalPeopleMain.HumourLoveForLife; name: qsTr("Юмор и жизнелюбие") }
    }

    ListModel {
        id: lifeMainModel

        ListElement { value: CFPersonalLifeMain.FamilyChildren; name: qsTr("Семья и дети") }
        ListElement { value: CFPersonalLifeMain.CareerMoney; name: qsTr("Карьера и деньги") }
        ListElement { value: CFPersonalLifeMain.EntertainmentLeisure; name: qsTr("Развлечения и отдых") }
        ListElement { value: CFPersonalLifeMain.ScienceResearch; name: qsTr("Наука и исследования") }
        ListElement { value: CFPersonalLifeMain.ImprovingWorld; name: qsTr("Совершенствование мира") }
        ListElement { value: CFPersonalLifeMain.PersonalDevelopment; name: qsTr("Саморазвитие") }
        ListElement { value: CFPersonalLifeMain.BeautyArt; name: qsTr("Красота и искусство") }
        ListElement { value: CFPersonalLifeMain.FameInfluence; name: qsTr("Слава и влияние") }
    }

    ListModel {
        id: smokingModel

        ListElement { value: CFPersonalSmoking.VeryNegative; name: qsTr("Резко негативное") }
        ListElement { value: CFPersonalSmoking.Negative; name: qsTr("Негативное") }
        ListElement { value: CFPersonalSmoking.Compromisable; name: qsTr("Компромиссное") }
        ListElement { value: CFPersonalSmoking.Neutral; name: qsTr("Нейтральное") }
        ListElement { value: CFPersonalSmoking.Positive; name: qsTr("Положительное") }
    }

    ListModel {
        id: alcoholModel

        ListElement { value: CFPersonalAlcohol.VeryNegative; name: qsTr("Резко негативное") }
        ListElement { value: CFPersonalAlcohol.Negative; name: qsTr("Негативное") }
        ListElement { value: CFPersonalAlcohol.Compromisable; name: qsTr("Компромиссное") }
        ListElement { value: CFPersonalAlcohol.Neutral; name: qsTr("Нейтральное") }
        ListElement { value: CFPersonalAlcohol.Positive; name: qsTr("Положительное") }
    }
}
