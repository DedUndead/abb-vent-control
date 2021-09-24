'use strict'

const { MongoClient, Db, Collection } = require('mongodb');

/**
 * Perform connection to the database and get promise
 * @param   {string}  db_name Database name
 * @param   {string}  url     MongoDB url
 * @returns {Promise}	      Promise containing database connection result
 */
module.exports.connect_to_mongo = async (db_name, url = 'mongodb://localhost:27017') => {
	const client = new MongoClient(url);
	return client.connect().then(() => client.db(db_name));
}

/**
 * Insert new object to collection
 * @param   {Db}      db 		      Target database name
 * @param   {string}  collection_name Target collection name
 * @param   {object}  data 			  Data to insert
 * @returns {Promise}                 Promise containing inserted data
 */
 module.exports.insert_object = async (db, collection_name, data) => {
	const collection = db.collection(collection_name);

	return collection.insertOne(data).then(() => data);
}

/**
 * Delete object from collection by parameters
 * @param   {Db}      db 		      Target database name
 * @param   {string}  collection_name Target collection name
 * @param   {object}  delete_data     Object containing parameters to delete doceumnts
 * @returns {Promise}                 Promise containing number of deleted documents
 */
module.exports.delete_object = async (db, collection_name, delete_data) => {
	const collection = db.collection(collection_name);

	return collection.deleteMany(delete_data).then(result => result.deletedCount);
}

/**
 * Get all elements in specified collection by specifying search data
 * @param   {Db}      db 		      Target database name
 * @param   {string}  collection_name Target collection name
 * @param   {object}  search_data     Target object
 * @returns {Promise}                 Promise containing array of elements
 */
module.exports.get_elements = async (db, collection_name, search_data) => {
	const collection = db.collection(collection_name);

	return collection.find(search_data).toArray();
}

/**
 * Aggregate data in collection by pipeline
 * @param   {Db}       db 		       Target database name
 * @param   {string}   collection_name Target collection name
 * @param   {object[]} pipeline        Pipeline
 * @returns {Promise}                  Promise containing array of elements
 */
module.exports.aggregate = async (db, collection_name, pipeline) => {
	const collection = db.collection(collection_name);

	return collection.aggregate(pipeline).toArray();
}

/**
 * Find minimum value of the specified key
 * @param   {Db}     db 		     Target database name
 * @param   {string} collection_name Target collection name
 * @param   {string} key             Target key
 * @returns {Promise} 				 Promise containing min value
 */
module.exports.get_min_value = async (db, collection_name, key) => {
	let pipeline = [{ 
		'$group': {
			'_id': null,
			'min': { '$min': `\$${key}` }
		}
	}];

	return exports.aggregate(
		db, collection_name, pipeline
	).then(result =>  result[0].min);
}

/**
 * Find maximum value of the specified key
 * @param   {Db}     db 		     Target database name
 * @param   {string} collection_name Target collection name
 * @param   {string} key             Target key
 * @returns {Promise} 				 Promise containing max value
 */
module.exports.get_max_value = async (db, collection_name, key) => {
	let pipeline = [{ 
		'$group': {
			'_id': null,
			'max': { '$max': `\$${key}` }
		}
	}];

	return exports.aggregate(
		db, collection_name, pipeline
	).then(result => result[0].max);
}

/**
 * Find maximum value of the specified key
 * @param   {Db}     db 		     Target database name
 * @param   {string} collection_name Target collection name
 * @param   {string} key             Target key
 * @returns {Promise} 				 Promise containing average value
 */
module.exports.get_average_value = async (db, collection_name, key) => {
	let pipeline = [{ 
		'$group': {
			'_id': null,
			'avg': { '$avg': `\$${key}` }
		}
	}];

	return exports.aggregate(
		db, collection_name, pipeline
	).then(result => result[0].avg);
}

/**
 * Get last N newest records from the collection
 * @param   {Db}      db 		      Target database name
 * @param   {string}  collection_name Target collection name
 * @param   {number}  n               Number of records
 * @returns {Promise}                 Promise containing last record
 */
module.exports.get_n_newest_records = async (db, collection_name, n) => {
	const collection = db.collection(collection_name);
	return collection.find().sort({_id: -1}).limit(n).toArray();
}

/**
 * Get number of element
 * This works faster than get_element().length
 * @param   {Db}      db 		      Target database name
 * @param   {string}  collection_name Target collection name
 * @returns {Promise}                 Promise containing number of matched elements
 */
module.exports.size_of = async (db, collection_name) => {
	const collection = db.collection(collection_name);
	return collection.countDocuments(); 
}