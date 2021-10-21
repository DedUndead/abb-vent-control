module.exports = {
    clearCollection: async function (DBclient, dbName, collect) {
        await DBclient.connect();
        DBclient.db(dbName).dropCollection(collect);
        console.log(`Collection ${collect} was dropped`);
    },
    insertData: async function (DBclient, dbName, collect, values) {
        try {
            await DBclient.connect();
            const database = DBclient.db(dbName);
            const collection = database.collection(collect);

            const doc = {
                member: values.member,
                email: values.email,
                password: values.password,
                rights: values.rights
            }
            const result = await collection.insertOne(doc);
        }
        catch (err) { console.log(err); }
    },
    retrieveData: async function (DBclient, dbName, collect) {
        try {
            await DBclient.connect();
            const database = DBclient.db(dbName);
            const collection = database.collection(collect);

            let data = await collection.find({}).toArray();
            return data;
        }
        catch (err) {
            console.log(err);
        }
    }
}