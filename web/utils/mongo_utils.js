'use strict'

const { MongoClient } = require('mongodb');

module.exports = class MongoDB {
	/**
	 * Initialize MongoClient with url
	 * @param {string} url Target url
	 */
	constructor(url = 'mongodb://localhost:27017') {
		this.client = new MongoClient(url);
	}

	/**
	 * Set new database to client
	 * @param {string} db_name Target db
	 */
	async switch_db(db_name) {
		this.db = this.client.db(db_name);
	}

	/**
	 * Make initial connection to database
	 * @param {string} db_name Target db
	 */
	async connect(db_name) {
		await this.client.connect();
		this.switch_db(db_name);
	}

	/**
	 * Insert new object to collection
	 * @param   {string}  collection_name 
	 * @param   {string}  collection_name Target collection name
     * @param   {object}  data 		      Data to insert
	 * @returns {Promise}                 Promise containing inserted data
	 */
	async insert_object(collection_name, data) {
		const collection = this.db.collection(collection_name);

		return collection.insertOne(data).then(() => data);
	}

	/**
	 * Delete object from collection by parameters
	 * @param   {string}  collection_name Target collection name
	 * @param   {object}  delete_data     Object containing parameters to delete doceumnts
	 * @returns {Promise}                 Promise containing number of deleted documents
	 */
	async delete_object(collection_name, delete_data) {
		const collection = this.db.collection(collection_name);

		return collection.deleteMany(delete_data).then(result => result.deletedCount);
	}

	/**
	 * Get all elements in specified collection by specifying search data
	 * @param   {string}  collection_name Target collection name
	 * @param   {object}  search_data     Target object
	 * @returns {Promise}                 Promise containing array of elements
	 */
	async get_elements(collection_name, search_data) {
		const collection = this.db.collection(collection_name);

		return collection.find(search_data).toArray();
	}

	/**
	 * Aggregate data in collection by pipeline
	 * @param   {string}   collection_name Target collection name
	 * @param   {object[]} pipeline        Pipeline
	 * @returns {Promise}                  Promise containing array of elements
	 */
	async aggregate(collection_name, pipeline) {
		const collection = this.db.collection(collection_name);

		return collection.aggregate(pipeline).toArray();
	}

	/**
	 * Find minimum value of the specified key
	 * @param   {string} collection_name Target collection name
	 * @param   {string} key             Target key
	 * @returns {Promise} 				 Promise containing min value
	 */
	async get_min_value(collection_name, key) {
		let pipeline = [{ 
			'$group': {
				'_id': null,
				'min': { '$min': `\$${key}` }
			}
		}];
	
		return this.aggregate(
			collection_name, pipeline
		).then(result =>  result[0].min);
	}

	/**
	 * Find maximum value of the specified key
	 * @param   {string} collection_name Target collection name
	 * @param   {string} key             Target key
	 * @returns {Promise} 				 Promise containing max value
	 */
	async get_max_value(collection_name, key) {
		let pipeline = [{ 
			'$group': {
				'_id': null,
				'max': { '$max': `\$${key}` }
			}
		}];

		return this.aggregate(
			collection_name, pipeline
		).then(result => result[0].max);
	}

	/**
	 * Find average value of the specified key
	 * @param   {string} collection_name Target collection name
	 * @param   {string} key             Target key
	 * @returns {Promise} 				 Promise containing average value
	 */
	async get_average_value(collection_name, key) {
		let pipeline = [{ 
			'$group': {
				'_id': null,
				'avg': { '$avg': `\$${key}` }
			}
		}];
	
		return this.aggregate(
			collection_name, pipeline
		).then(result => result[0].avg);
	}

	/**
	 * Get last N newest records from the collection
	 * @param   {string}  collection_name Target collection name
	 * @param   {number}  n               Number of records
	 * @returns {Promise}                 Promise containing last record
	 */
	async get_n_newest_records(collection_name, n) {
		const collection = this.db.collection(collection_name);

		return collection.find().sort({_id: -1}).limit(n).toArray();
	}

	/**
	 * Get last N oldest records from the collection
	 * @param   {string}  collection_name Target collection name
	 * @param   {number}  n               Number of records
	 * @returns {Promise}                 Promise containing old record
	 */
	async get_n_oldest_records(collection_name, n) {
		const collection = this.db.collection(collection_name);

		return collection.find().sort({_id: 1}).limit(n).toArray();
	}

	/**
	 * Get number of element
	 * This works faster than get_element().length
	 * @param   {string}  collection_name Target collection name
	 * @returns {Promise}                 Promise containing number of matched elements
	 */
	async size_of(collection_name) {
		const collection = this.db.collection(collection_name);

		return collection.countDocuments(); 
	}
}