// Args: user_list, group_count

var items = [];
var userArray = Args.user_list.split(",");
var iteration = 0;

var getSubscriptionsQuery = {
    "user_id": 0,
    "extended": 1,
    "fields": "activity",
    "count": Args.group_count,
    "lang": "ru"
};

while (iteration < userArray.length) {
    getSubscriptionsQuery.user_id = userArray[iteration];
    items.push({"userId": userArray[iteration], "subscriptions": API.users.getSubscriptions(getSubscriptionsQuery).items});
    iteration = iteration + 1;
}

return items;