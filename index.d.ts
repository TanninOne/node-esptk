declare module 'esptk' {
  class ESPFile {
    constructor(filePath: string);
    setLightFlag(enabled: boolean): void;
    isMaster: boolean;
    isLight: boolean;
    isDummy: boolean;
    author: string;
    description: string;
    masterList: string[];
    revision: number;
  }
  export default ESPFile;
}
