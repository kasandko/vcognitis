//Args: group_id, offset, maxIterationsCount

var fields = "first_name,last_name,deactivated,sex,bdate,city,country,photo_max,domain,last_seen,relation,personal,universities,schools,maiden_name,can_write_private_message";
var items = [];
var iteration = 0;

var getMembersQuery = {
    "group_id": Args.group_id,
    "offset": Args.offset,//Args.offset,
    "count": 1000,
    "lang": "ru",
    "fields": fields
};

var memberCount = 0;
while (getMembersQuery.offset != -1 && iteration < parseInt(Args.maxIterationsCount)) {
    var lastItemsLength = items.length;
    var getMembersResponse = API.groups.getMembers(getMembersQuery);
    memberCount = getMembersResponse.count; 
    items = items + getMembersResponse.items;
    iteration = iteration + 1;
    getMembersQuery.offset = lastItemsLength == items.length ? -1 : parseInt(getMembersQuery.offset) + 1000;
}

return {"items": items, "nextOffset": getMembersQuery.offset, "count": memberCount};