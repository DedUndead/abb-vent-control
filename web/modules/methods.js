module.exports = {
    clearCollection: async function (DBclient, dbName, collect) {
        await DBclient.connect();
        DBclient.db(dbName).dropCollection(collect);
        console.log(`Collection ${collect} was dropped`);
    },
    

}