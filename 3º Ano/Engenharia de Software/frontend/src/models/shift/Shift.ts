import { ISOtoString } from "@/services/ConvertDateService";

export default class Shift {
    id: number | null = null;
    activityId: number | null = null;
    participantsLimit!: number;
    location!: string;
    startTime!: string;
    formattedStartTime!: string;
    endTime!: string;
    formattedEndTime!: string;

    constructor(jsonObj?: Shift) {
        if(jsonObj){
            this.id = jsonObj.id;
            this.activityId = jsonObj.activityId;
            this.participantsLimit = jsonObj.participantsLimit;
            this.location = jsonObj.location;
            this.startTime = jsonObj.startTime;
            if (jsonObj.startTime)
                this.formattedStartTime = ISOtoString(jsonObj.startTime);
            this.endTime = jsonObj.endTime;
            if (jsonObj.endTime)
                this.formattedEndTime = ISOtoString(jsonObj.endTime);
        }
    }
}