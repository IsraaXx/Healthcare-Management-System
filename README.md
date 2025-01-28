# Healthcare Management System  

## Overview  
This project implements a simple **Healthcare Management System** where an administrator can:  
- Manage doctor and appointment records.  
- Search for appointments of a specific doctor.  
- Perform CRUD operations (Create, Read, Update, Delete).  
- Execute structured queries on the data files.  

The system leverages **primary and secondary indexing** to optimize data retrieval and organization.  

---

## Features  
### Main Functionalities  
- **Add New Doctor**  
- **Add New Appointment**  
- **Update Doctor Name (Doctor ID)**  
- **Update Appointment Date (Appointment ID)**  
- **Delete Appointment (Appointment ID)**  
- **Delete Doctor (Doctor ID)**  
- **Print Doctor Info (Doctor ID)**  
- **Print Appointment Info (Appointment ID)**  
- **Write Query**  
- **Exit**  

---

## Data Management  
### Doctors Data File  
- **Doctor ID**: `Char[15]` (Primary Key)  
- **Doctor Name**: `Char[30]`  
- **Address**: `Char[30]`  

### Appointments Data File  
- **Appointment ID**: `Char[15]` (Primary Key)  
- **Appointment Date**: `Char[30]`  
- **Doctor ID**: `Char[15]` (Secondary Key)  

---

## Indexing Techniques  
1. **Primary Index**  
   - Based on `Doctor ID` for doctors.  
   - Based on `Appointment ID` for appointments.  

2. **Secondary Index**  
   - Based on `Doctor ID` (for appointments).  
   - Based on `Doctor Name` (for doctors).  

3. **Linked List** for secondary indexes.  

---

## Record Management  
1. **Add Records**  
   - Check the **availability list** before writing.  
   - If the record already exists, it won't be added.  

2. **Delete Records**  
   - Records are **marked with a `*`** instead of being physically deleted.  
   - Updates the availability list accordingly.  

3. **Update Records**  
   - Non-key fields can be updated, ensuring they do not exceed allocated sizes.  

---

## Query Operations  
### Supported Queries  
- `SELECT ALL FROM Doctors WHERE Doctor ID='xxx';`  
  - Uses the primary index.  
- `SELECT ALL FROM Appointments WHERE Doctor ID='xxx';`  
  - Uses the secondary index.  
- `SELECT Doctor Name FROM Doctors WHERE Doctor ID='xxx';`  
  - Uses the secondary index.  

### Search Implementation  
- **Binary Search** for efficient index lookup.  
- **Bind secondary indexes to the primary index** (not directly to file addresses).  

